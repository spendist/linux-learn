<div aligen="center">
<h1> 备忘录</h1>

# weChat.c
这个程序用C语言编写， 引用了 libjson-c, libcurl 两个系统自带的库，编译时可以参照这个Makefile编写
```makefile
CC=gcc
CCOPTIONS = -g -Wall

LIBS= -lcurl -ljson-c


all: weiChat.o
	$(CC) -o weiChat.exe weiChat.o $(LIBS) $(CCOPTIONS)

clean:
	rm *.o *.exe
```
可以很方便的移植到 openwrt， windows10 等环境中。
