server : server.o user.o net.o
	cc server.o user.o net.o -o server

client : client.o user.o net.o
	cc client.o user.o net.o -o client

server.o : server.c server.h net.h user.h
	cc -c server.c

client.o : client.c net.h user.h
	cc -c client.c

user.o : user.c user.h
	cc -c user.c

net.o : net.c net.h
	cc -c net.c

clean :
	rm server.o client.o user.o net.o
