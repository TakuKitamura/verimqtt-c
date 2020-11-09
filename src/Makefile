OBJ=common.o connect.o const.o debug.o disconnect.o main.o publish.o ffi.o kremlinit.o callMain.o
KREMLIB=./kremlin/kremlib
INCLUDE=./kremlin/include

CFLAGS=

CC=gcc -I $(KREMLIB) -I $(INCLUDE) $(CFLAGS)


verimqtt : $(OBJ)
	make -C ./kremlin/kremlib
	$(CC) -o ../$@  $^ ./kremlin/kremlib/dist/generic/libkremlib.a
	
common.o : Common.c
	$(CC) -c $^

connect.o : Connect.c
	$(CC) -c $^

const.o : Const.c
	$(CC) -c $^

debug.o : Debug.c
	$(CC) -c $^

disconnect.o : Disconnect.c
	$(CC) -c $^

main.o : Main.c
	$(CC) -c $^

publish.o : Publish.c
	$(CC) -c $^

ffi.o : ffi.c
	$(CC) -c $^

kremlinit.o : kremlinit.c
	$(CC) -c $^

callMain.o : callMain.c
	$(CC) -c $^