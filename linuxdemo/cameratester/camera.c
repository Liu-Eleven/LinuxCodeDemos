
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "jpeglib.h"
#include "drv_display_sun4i.h"
#include "dragonboard_inc.h"
#include "convert.h"
#include "camera.h"

struct buffer
{
	void   *start;
	size_t length;
};

struct size
{
	int width;
	int height;
};

static int fd;

static int csi_format;
static int fps;
static int req_frame_num;

static struct buffer *buffers = NULL;
static int nbuffers = 0;

static struct size input_size;
static struct size disp_size;

static int disp;
static int layer;
static __disp_layer_info_t layer_para;
static int screen_width;
static int screen_height;
static __disp_pixel_fmt_t disp_format;
static __disp_pixel_mod_t disp_seq;
static __disp_pixel_seq_t disp_mode;


#define WIDTH    640
#define HEIGHT    480
unsigned char rgb_buffer[1 * 1024 * 1024];

static int read_frame(void)
{
	struct v4l2_buffer buf;
	char *buffer=NULL;
	unsigned char* y;  
	unsigned char* u;  
	unsigned char* v;  
	
	memset(&buf, 0, sizeof(struct v4l2_buffer));
	buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	ioctl(fd, VIDIOC_DQBUF, &buf);

	//save RGB format image datas  
	//unsigned char* dst=(unsigned char*) malloc(3 * disp_size.width * disp_size.height); 
	unsigned char* dst = rgb_buffer;

	buffer = (unsigned char *)buffers[buf.index].start;
 	y = buffer;  
	u = buffer + disp_size.width * disp_size.height;  
	v = u + disp_size.width * disp_size.height / 4; 

	memset(rgb_buffer, 0 ,sizeof(rgb_buffer));
 	ConvertYUV2RGB(y, u, v, dst, disp_size.width, disp_size.height); 
	encode_jpeg("picture.jpg", dst, disp_size.width, disp_size.height);
	
	//free(dst);
	
	ioctl(fd, VIDIOC_QBUF, &buf);
	return 1;
}


int do_camera_capture_init(void)
{
	char dev_name[32];
	struct v4l2_input inp;
	struct v4l2_format fmt;
	struct v4l2_streamparm parms;
	struct v4l2_requestbuffers req;
	int i;
	enum v4l2_buf_type type;

	csi_format = V4L2_PIX_FMT_NV12;
	fps = 30;
	req_frame_num = 10;

	input_size.width = WIDTH; //320; 
	input_size.height = HEIGHT; // 240
	
	disp_format = DISP_FORMAT_YUV420;
	disp_seq = DISP_SEQ_UVUV;
	disp_mode = DISP_MOD_NON_MB_UV_COMBINED;

	
	strncpy(dev_name, "/dev/video0", 32);
	db_debug("open %s\n", dev_name);
	if ((fd = open(dev_name, O_RDWR | O_NONBLOCK, 0)) < 0) 
	{
		db_error("can't open %s(%s)\n", dev_name, strerror(errno));
		goto open_err;
	}

	inp.index = 0;
	inp.type = V4L2_INPUT_TYPE_CAMERA;

	/* set input input index */
	if (ioctl(fd, VIDIOC_S_INPUT, &inp) == -1) 
	{
		db_error("VIDIOC_S_INPUT error\n");
		goto err;
	}

	/* set image format */
	memset(&fmt, 0, sizeof(struct v4l2_format));
	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = input_size.width;
	fmt.fmt.pix.height      = input_size.height;
	fmt.fmt.pix.pixelformat = csi_format;
	fmt.fmt.pix.field       = V4L2_FIELD_NONE;
	if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) 
	{
		db_error("set image format failed\n");
		goto err;
	}

	disp_size.width = fmt.fmt.pix.width;
	disp_size.height = fmt.fmt.pix.height;
	db_debug("image input width #%d height #%d, diplay width #%d height %d\n", 
			input_size.width, input_size.height, disp_size.width, disp_size.height);

	parms.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parms.parm.capture.timeperframe.numerator = 1;
	parms.parm.capture.timeperframe.denominator = fps;
	if (ioctl(fd, VIDIOC_S_PARM, &parms) == -1) 
	{
		db_error("set frequence failed\n");
		goto err;
	}

	/* request buffer */
	memset(&req, 0, sizeof(struct v4l2_requestbuffers));
	req.count  = req_frame_num;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	ioctl(fd, VIDIOC_REQBUFS, &req);

	buffers = calloc(req.count, sizeof(struct buffer));
	for (nbuffers = 0; nbuffers < req.count; nbuffers++) 
	{
		struct v4l2_buffer buf;

		memset(&buf, 0, sizeof(struct v4l2_buffer));
		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = nbuffers;

		if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) 
		{
			db_error("VIDIOC_QUERYBUF error\n");
            		goto buffer_rel; 
		}

		buffers[nbuffers].start  = mmap(NULL, buf.length, 
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
		buffers[nbuffers].length = buf.length;
		if (buffers[nbuffers].start == MAP_FAILED) 
		{
			db_error("mmap failed\n");
			goto buffer_rel;
		}
	}

	InitConvertTable();
	//start_capturing();
	
	return 0;
	
stream_off:
	ioctl(fd, VIDIOC_STREAMOFF, &type);
unmap:
	for (i = 0; i < nbuffers; i++) 
	{
		munmap(buffers[i].start, buffers[i].length);
	}
buffer_rel:
	free(buffers);
err:
	close(fd);
open_err:
	return -1;
}



void stop_capturing(void)
{
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ioctl(fd, VIDIOC_STREAMOFF, &type);
}

void start_capturing(void)
{
	unsigned int i;
	enum v4l2_buf_type type;

	for (i = 0; i < nbuffers; i++) 
	{
		struct v4l2_buffer buf;

		memset(&buf, 0, sizeof(struct v4l2_buffer));
		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = i;
		if (ioctl(fd, VIDIOC_QBUF, &buf) == -1) 
		{
			db_error("VIDIOC_QBUF error\n");
		}
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) 
	{
		db_error("VIDIOC_STREAMON error\n");
	}
}


int do_camera_capture(void)
{
	start_capturing();
	while (1) 
	{
		fd_set fds;
		struct timeval tv;
		int r;

		FD_ZERO(&fds);
		FD_SET(fd, &fds);

		/* timeout */
		tv.tv_sec  = 2;
		tv.tv_usec = 0;

		r = select(fd + 1, &fds, NULL, NULL, &tv);
		if (r == -1) 
		{
			if (errno == EINTR) 
			{
				continue;
			}

			db_error("select error\n");
		}

		if (r == 0) 
		{
			db_error("select timeout\n");			
		}

		if (read_frame()) 
		{
			break;
		}		
	}
	stop_capturing();
	
	return 0;
}



int do_camera_capture_done(void)
{
	int i;
	
	//stop_capturing();

	for (i = 0; i < nbuffers; i++) 
	{
		munmap(buffers[i].start, buffers[i].length);
	}

	free(buffers);
	close(fd);

	return -1;
}


