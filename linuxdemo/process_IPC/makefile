objects = main.o ngx_channel.o ngx_process.o ngx_process_cycle.o
process_demo : $(objects)                                                   
	gcc -o process_demo $(objects)                                        


$(objects):ngx_channel.h ngx_process.h

.PHONY : clean                                            
clean :                                                                  
	-rm process_demo  $(objects)                                         
