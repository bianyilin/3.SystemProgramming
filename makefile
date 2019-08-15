CC := gcc
SHDIR := ../common 
CFLAGS := -Wall -g -Os -lpthread -I $(SHDIR) //报警 库 头文件
OBJS = master.o $(SHDIR)/common.o //对象 $（）取变量值


all: pihealthd //master  client
	@echo "This is OK"

pihealthd: $(OBJS)
	@$(CC) -o $@ $(CFLAGS) $(OBJS)//gcc-o

$(OBJS) : %.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

#.PHONY: clean //伪目标
clean:
	@rm -f *.o pihealthd //*.o全删
	@rm -f ../common/*.o
	@echo Done cleaning

install: pihealthd install.sh pihealthd.conf.sample
	@test -f /etc/pihealthd.conf || cp pihealthd.conf.sample /etc/pihealthd.conf 
	//test==[[  ]] 测试 如果不存在拷贝 存在往下执行bash
	@bash install.sh
	@cp -f pihealthd /usr/bin //-f (文件 格式)
	@echo "PiHealthd installed"
