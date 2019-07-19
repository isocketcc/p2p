CFLAGS := -g -std=c99 -Wall -Wno-implicit-function-declaration
TARGETS := server client
all: $(TARGETS)

client : client.c endpoint.c   endpoint_list.c
	gcc $^ -o $@ $(CFLAGS) -pthread
server : server.c endpoint.c  endpoint_list.c
	gcc $^ -o $@ $(CFLAGS)

clean: 
	rm -f $(TARGETS) *.o
.PHONY: clean
