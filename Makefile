CC = clang
CFLAGS = -Wall -g
TARGET = webserver
OBJS = server_main.o web_server.o http.o error.o

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

server_main.o: server_main.c web_server.h
	$(CC) $(CFLAGS) -c server_main.c -o server_main.o

web_server.o: web_server.c web_server.h http.h error.h
	$(CC) $(CFLAGS) -c web_server.c -o web_server.o

http.o: http.c http.h
	$(CC) $(CFLAGS) -c http.c -o http.o

error.o: error.c error.h http.h
	$(CC) $(CFLAGS) -c error.c -o error.o

clean:
	rm -f $(TARGET) $(OBJS)
