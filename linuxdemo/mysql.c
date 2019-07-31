
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// yum install libmysqlclient-dev

// find / -name mysql.h
// /usr/include/mysql/mysql.h

// gcc -o mysql mysql.c -L/usr/lib64/mysql -lmysqlclient

#include "mysql/mysql.h"

#if 0

typedef struct st_mysql
{
  NET           net;                    /* Communication parameters */
  unsigned char *connector_fd;          /* ConnectorFd for SSL */
  char          *host,*user,*passwd,*unix_socket,*server_version,*host_info;
  char          *info, *db;
  struct charset_info_st *charset;
  MYSQL_FIELD   *fields;
  MEM_ROOT      field_alloc;
  my_ulonglong affected_rows;
  my_ulonglong insert_id;               /* id if insert on table with NEXTNR */
  my_ulonglong extra_info;              /* Not used */
  unsigned long thread_id;              /* Id for connection in server */
  unsigned long packet_length;
  unsigned int  port;
  unsigned long client_flag,server_capabilities;
  unsigned int  protocol_version;
  unsigned int  field_count;
  unsigned int  server_status;
  unsigned int  server_language;
  unsigned int  warning_count;
  struct st_mysql_options options;
  enum mysql_status status;
  my_bool       free_me;                /* If free in mysql_close */
  my_bool       reconnect;              /* set to 1 if automatic reconnect */
 
  /* session-wide random string */
  char          scramble[SCRAMBLE_LENGTH+1];
 
 /*
   Set if this is the original connection, not a master or a slave we have
   added though mysql_rpl_probe() or mysql_set_master()/ mysql_add_slave()
 */
  my_bool rpl_pivot;
  /*
    Pointers to the master, and the next slave connections, points to
    itself if lone connection.
  */
  struct st_mysql* master, *next_slave;
 
  struct st_mysql* last_used_slave; /* needed for round-robin slave pick */
 /* needed for send/read/store/use result to work correctly with replication */
  struct st_mysql* last_used_con;
 
  LIST  *stmts;                     /* list of all statements */
  const struct st_mysql_methods *methods;
  void *thd;
  /*
    Points to boolean flag in MYSQL_RES  or MYSQL_STMT. We set this flag
    from mysql_stmt_close if close had to cancel result set of this object.
  */
  my_bool *unbuffered_fetch_owner;
  /* needed for embedded server - no net buffer to store the 'info' */
  char *info_buffer;
  void *extension;
} MYSQL;

typedef struct st_mysql_res {
  my_ulonglong  row_count;
  MYSQL_FIELD   *fields;
  MYSQL_DATA    *data;
  MYSQL_ROWS    *data_cursor;
  unsigned long *lengths;               /* column lengths of current row */
  MYSQL         *handle;                /* for unbuffered reads */
  const struct st_mysql_methods *methods;
  MYSQL_ROW     row;                    /* If unbuffered read */
  MYSQL_ROW     current_row;            /* buffer to current row */
  MEM_ROOT      field_alloc;
  unsigned int  field_count, current_field;
  my_bool       eof;                    /* Used by mysql_fetch_row */
  /* mysql_stmt_close() had to cancel this result */
  my_bool       unbuffered_fetch_cancelled;  
  void *extension;
} MYSQL_RES;

MySQL 有如下C语言API：
函数

描述
mysql_affected_rows()    返回上次UPDATE、DELETE或INSERT查询更改／删除／插入的行数。
mysql_autocommit()       切换 autocommit模式，ON/OFF
mysql_change_user()      更改打开连接上的用户和数据库。
mysql_charset_name()     返回用于连接的默认字符集的名称。
mysql_close()            关闭服务器连接。
mysql_commit()           提交事务。
mysql_connect()          连接到MySQL服务器。该函数已不再被重视，使用mysql_real_connect()取代。
mysql_create_db()        创建数据库。该函数已不再被重视，使用SQL语句CREATE DATABASE取而代之。
mysql_data_seek()        在查询结果集中查找属性行编号。
mysql_debug()            用给定的字符串执行DBUG_PUSH。
mysql_drop_db()撤销数据库。该函数已不再被重视，使用SQL语句DROP DATABASE取而代之。
mysql_dump_debug_info()让服务器将调试信息写入日志。
mysql_eof()确定是否读取了结果集的最后一行。该函数已不再被重视，可以使用mysql_errno()或mysql_error()取而代之。
mysql_errno()返回上次调用的MySQL函数的错误编号。
mysql_error()返回上次调用的MySQL函数的错误消息。
mysql_escape_string()为了用在SQL语句中，对特殊字符进行转义处理。
mysql_fetch_field()返回下一个表字段的类型。
mysql_fetch_field_direct()给定字段编号，返回表字段的类型。
mysql_fetch_fields()返回所有字段结构的数组。
mysql_fetch_lengths()返回当前行中所有列的长度。
mysql_fetch_row()从结果集中获取下一行
mysql_field_seek()将列光标置于指定的列。
mysql_field_count()返回上次执行语句的结果列的数目。
mysql_field_tell()返回上次mysql_fetch_field()所使用字段光标的位置。
mysql_free_result()释放结果集使用的内存。
mysql_get_client_info()以字符串形式返回客户端版本信息。
mysql_get_client_version()以整数形式返回客户端版本信息。
mysql_get_host_info()返回描述连接的字符串。
mysql_get_server_version()以整数形式返回服务器的版本号。
mysql_get_proto_info()返回连接所使用的协议版本。
mysql_get_server_info()返回服务器的版本号。
mysql_info()返回关于最近所执行查询的信息。
mysql_init()获取或初始化MYSQL结构。
mysql_insert_id()返回上一个查询为AUTO_INCREMENT列生成的ID
mysql_kill()杀死给定的线程。
mysql_library_end()最终确定MySQL C API库。
mysql_library_init()初始化MySQL C API库。
mysql_list_dbs()返回与简单正则表达式匹配的数据库名称。
mysql_list_fields()返回与简单正则表达式匹配的字段名称。
mysql_list_processes()返回当前服务器线程的列表。
mysql_list_tables()返回与简单正则表达式匹配的表名。
mysql_more_results()检查是否还存在其他结果。
mysql_next_result()在多语句执行过程中返回/初始化下一个结果。
mysql_num_fields()返回结果集中的列数。
mysql_num_rows()返回结果集中的行数。
mysql_options()为mysql_connect()设置连接选项。
mysql_ping()检查与服务器的连接是否工作，如有必要重新连接。
mysql_query()执行指定为“以Null终结的字符串”的SQL查询。
mysql_real_connect()连接到MySQL服务器。
mysql_real_escape_string()考虑到连接的当前字符集，为了在SQL语句中使用，对字符串中的特殊字符进行转义处理。
mysql_real_query()执行指定为计数字符串的SQL查询。
mysql_refresh()刷新或复位表和高速缓冲。
mysql_reload()通知服务器再次加载授权表。
mysql_rollback()回滚事务。
mysql_row_seek()
使用从mysql_row_tell()返回的值，查找结果集中的行偏移。
mysql_row_tell()返回行光标位置。
mysql_select_db()选择数据库。
mysql_server_end()最终确定嵌入式服务器库。
mysql_server_init()初始化嵌入式服务器库。
mysql_set_server_option()为连接设置选项（如多语句）。
mysql_sqlstate()返回关于上一个错误的SQLSTATE错误代码。
mysql_shutdown() 关闭数据库服务器。
mysql_stat() 以字符串形式返回服务器状态。
mysql_store_result() 检索完整的结果集至客户端。
mysql_thread_id()  返回当前线程ID。
mysql_thread_safe()  如果客户端已编译为线程安全的，返回1。
mysql_use_result()  初始化逐行的结果集检索。
mysql_warning_count()  返回上一个SQL语句的告警数。

#endif

int main(int argc, char **argv)
{
	MYSQL conn;
	int ret;

	mysql_init(&conn);
	if (mysql_real_connect(&conn, "localhost", "root", "123456", "jason_db", 3306, NULL, 0))
	{
		printf("connectin success\n");

		
		ret = mysql_query(&conn, "INSERT INTO persons(name, age) VALUES('walking', 36)");
		if (!ret)
		{
			printf("inserted %lu rows\n", (unsigned long)mysql_affected_rows(&conn));
		}
		else
		{
			printf("inserted error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
		}

		const char *str = "select * from persons";
		if (mysql_real_query(&conn, str, strlen(str)))
		{
			printf("mysql_real_query failed: %s, sql: %s\n", mysql_error(&conn), str);
			exit(3);
		}

		MYSQL_RES *res = mysql_store_result(&conn);
		if (!res)
		{
			printf("mysql_store_result failed: %s", mysql_error(&conn));
			exit(4);
		}

		// 输出查询的结果
		MYSQL_ROW row, end_row;
		int columns = mysql_num_fields(res);
		while ((row = mysql_fetch_row(res)))
		{
			for (end_row = row + columns; row < end_row; ++row)
			{
				printf("%s\t", row ? (char *)*row : "NULL");
			}
			printf("\n");
		}

		mysql_close(&conn);
	}
	else
	{
		printf("connectin failed\n");
		if (mysql_errno(&conn))
		{
			printf("connectin error %d: %s\n", mysql_errno(&conn), mysql_error(&conn));
		}
	}
	return EXIT_SUCCESS;
}
