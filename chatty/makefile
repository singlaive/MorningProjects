all: chatty_srv chatty_clt

chatty_%: src/chatty_%.o
#chatty_srv: src/chatty_srv.o
	cc -o $@ $^

#chatty_srv.o: src/chatty_srv.c
#	cc -c src/chatty_srv.c

#chatty_clt: src/chatty_clt.o
#	cc -o $@ $^

#chatty_clt.o: src/chatty_clt.c
#	cc -c src/chatty_clt.c
	
#%.o: src/%.c
	
PHONY: clean
clean:
	rm chatty_* src/*.o	
	
configure:
	echo Chatty configured.
