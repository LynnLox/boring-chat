server : server.o user.o net.o
	cc server.o user.o net.o -o server

server.o : server.c server.o net.h user.h
	cc -c server.c

user.o : user.c user.o
	cc -c user.c

net.o : net.c net.h
	cc -c net.c

clean :
	rm server.o user.o net.o
