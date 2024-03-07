#狗屎
# OBJECTS_SERVER = StdTcp.c StdThread.c DoubleLinkList.c testServer.c  Queue.c ThreadPool.c StdSqlite.c StdFile.c
# server:${OBJECTS_SERVER}
# 		gcc ${OBJECTS_SERVER} -o server -lsqlite3

# OBJECTS_CLIENT = StdTcp.c StdThread.c DoubleLinkList.c testClient.c Queue.c ThreadPool.c StdSqlite.c StdFile.c
# client:${OBJECTS_CLIENT}
# 		gcc ${OBJECTS_CLIENT} -o client -lsqlite3


#版本1 

# server:StdTcp.o StdThread.o DoubleLinkList.o testServer.o Queue.o ThreadPool.o StdSqlite.o StdFile.o
#  	gcc StdTcp.o StdThread.o DoubleLinkList.o testServer.o Queue.o ThreadPool.o StdSqlite.o StdFile.o -o server -lsqlite3

# StdTcp.o:StdTcp.c
# 	gcc -c StdTcp.c -o StdTcp.o

# DoubleLinkList.o:DoubleLinkList.c
# 	gcc -c DoubleLinkList.c -o DoubleLinkList.o

# testServer.o:testServer.c
# 	gcc -c testServer.c -o testServer.o

# Queue.o:Queue.c
# 	gcc -c Queue.c -o Queue.o

# ThreadPool.o:ThreadPool.c
# 	gcc -c ThreadPool.c -o ThreadPool.o

# StdSqlite.o:StdSqlite.c
# 	gcc -c StdSqlite.c -o StdSqlite.o

# StdFile.o:StdFile.c
# 	gcc -c StdFile.c -o StdFile.o

# clean:
# 	@rm -rf *.o server

#版本2
#自定于对象 $^ : 代表不重复的依赖文件  #@ : 所有的目标文件
# OBJ=StdTcp.o StdThread.o DoubleLinkList.o testServer.o Queue.o ThreadPool.o StdSqlite.o StdFile.o
# TARGET=server 
# linkLibrary=-lsqlite3

# $(TARGET):$(OBJ)
# 	$(CC) $^ -o $@ $(linkLibrary)

# StdTcp.o:StdTcp.c
# 	gcc -c $^ -o $@

# DoubleLinkList.o:DoubleLinkList.c
# 	gcc -c $^ -o $@

# testServer.o:testServer.c
# 	gcc -c $^ -o $@

# Queue.o:Queue.c
# 	gcc -c $^ -o $@

# ThreadPool.o:ThreadPool.c
# 	gcc -c $^ -o $@

# StdSqlite.o:StdSqlite.c
# 	gcc -c $^ -o $@

# StdFile.o:StdFile.c
# 	gcc -c $^ -o $@

# clean:
# 	@rm -rf *.o $(TARGET)

# #版本3
# #自定于对象 $^ : 代表不重复的依赖文件  #@ : 所有的目标文件
# #增加系统变量 $(CC) 代表 c语言   $(CXX)  
# OBJ=StdTcp.o StdThread.o DoubleLinkList.o testServer.o Queue.o ThreadPool.o StdSqlite.o StdFile.o
# TARGET=server 
# linkLibrary=-lsqlite3

# $(TARGET):$(OBJ)
# 	$(CC) $^ -o $@ $(linkLibrary)

# StdTcp.o:StdTcp.c
# 	$(CC) -c $^ -o $@

# DoubleLinkList.o:DoubleLinkList.c
# 	$(CC) -c $^ -o $@

# testServer.o:testServer.c
# 	$(CC) -c $^ -o $@

# Queue.o:Queue.c
# 	$(CC) -c $^ -o $@

# ThreadPool.o:ThreadPool.c
# 	$(CC) -c $^ -o $@

# StdSqlite.o:StdSqlite.c
# 	$(CC) -c $^ -o $@

# StdFile.o:StdFile.c
# 	$(CC) -c $^ -o $@

# clean:
# 	$(RM) *.o $(TARGET)


#版本4
#自定于对象 $^ : 代表不重复的依赖文件  #@ : 所有的目标文件
#增加系统变量 $(CC) 代表 c语言   $(CXX)  
#使用模式匹配 使用 % 来实现模式匹配 表示匹配任意多个非空字符 

# OBJ=StdTcp.o StdThread.o DoubleLinkList.o testServer.o Queue.o ThreadPool.o StdSqlite.o StdFile.o
# TARGET=server 

# OBJ2=StdTcp.o StdThread.o DoubleLinkList.o testClient.o Queue.o ThreadPool.o StdSqlite.o StdFile.o
# TARGET2=client 

# linkLibrary=-lsqlite3

# $(TARGET):$(OBJ)
# 	$(CC) $^ -g -o $@ $(linkLibrary)
# $(TARGET2):$(OBJ2)
# 	$(CC) $^ -g -o $@ $(linkLibrary)
# #模式匹配  %目标 ： %依赖
# %.o:%.c
# 	$(CC) -g -c $^ -o $@

# all:


# clean:
# 	$(RM) *.o $(TARGET) $(TARGET2)


#版本5
#自定于对象 $^ : 代表不重复的依赖文件  #@ : 所有的目标文件
#增加系统变量 $(CC) 代表 c语言   $(CXX)  
#使用模式匹配 使用 % 来实现模式匹配 表示匹配任意多个非空字符 
#patsubst %.c, %.o 将 所有.c 替换成 .o   在当前文件下wildcard ./*.c 匹配所有.c文件

# OBJ=$(patsubst %.c, %.o, $(wildcard ./*.c))
# TARGET=server 

# $(TARGET):$(OBJ)
# 	$(CC) $^ -o $@ -lsqlite3
# #模式匹配  %目标 ： %依赖
# %.o:%.c
# 	$(CC) -c $^ -o $@

# clean:
# 	$(RM) *.o $(TARGET)

# 自定义变量
# OBJ=$(patsubst %.c, %.o, $(wildcard ./*.c))
# TARGET=main

# # 指定文件路径 -L 搜索动态库的指定路径  
# LDFLAGS=-L./src_so2
# LIBS=-lSoMyTree

# #使用$(TARGET) 必须要加 '$' 符号
# # ^表示所有不重复的依赖项 
# $(TARGET): $(OBJ)
# #发布

# 	$(CC)  $^ $(LIBS) $(LDFLAGS) -o $@

# # 模式匹配  %目标 : % 依赖
# %.o:%.c
# 	$(CC) -c $^ -o $@


# # 伪目标
# .PHONY: clean

# # 清除编译的依赖文件 和 二进制文件 
# clean:
# #@ 表示不显示输出
# 	@$(RM) *.o $(TARGET)

# show:
# 	@echo $(CC)
# 	@echo $(RM)
# 	echo $(wildcard ./*.c)
# # 匹配 代表  $(wildcard ./*.c) patsubst匹配 第三个参数在当前文件夹下的所有.c 文件  将所有的.c 替换为.o
# 	@echo $(patsubst %.c, %.o , $(wildcard ./*.c))
	
#版本6
TARFET=server
TARGET=client
OBJ=testServer.o
OBJECT=testClient.o

LDFALGS=-L./src_so -L./src_so2
LIBS=-lAVLTree

$(TARFET):$(OBJ)
	$(CC) -g $^ $(LIBS) $(LDFALGS) -o $@ -lsqlite3 

$(TARGET):$(OBJECT)
	$(CC) -g $^ $(LIBS) $(LDFALGS) -o $@ -lsqlite3 

%.o:%.c
	gcc -g -c $^ -o $@

# 伪目标
.PHONY:clean

clean:
	@rm -rf *.o $(TARFET) $(TARGET)

all:server client

