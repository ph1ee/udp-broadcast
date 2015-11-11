BIN := udp-broadcast-server udp-broadcast-client

all: $(BIN)

udp-broadcast-server: udp-broadcast-server.o mkaddr.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

udp-broadcast-client: udp-broadcast-client.o mkaddr.o
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	-rm -rf $(BIN) *.o

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<


