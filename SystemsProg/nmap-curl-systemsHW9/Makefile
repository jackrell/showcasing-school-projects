CC=		gcc
CFLAGS=		-Wall -g -std=gnu99
LD=		gcc
LDFLAGS=	-L.
TARGETS=	nmapit curlit

all:		$(TARGETS)

#------------------------------------------------------------------------------
# Rules for object files and executables
#------------------------------------------------------------------------------

socket.o: socket.c socket.h
	$(CC) $(CFLAGS) -c -o $@ $<

nmapit.o: nmapit.c socket.h
	$(CC) $(CFLAGS) -c -o $@ $<

curlit.o: curlit.c socket.h
	$(CC) $(CFLAGS) -c -o $@ $<

nmapit:	nmapit.o socket.o
	$(LD) $(LDFLAGS) -o $@ $^

curlit: curlit.o socket.o
	$(LD) $(LDFLAGS) -o $@ $^


#------------------------------------------------------------------------------
# DO NOT MODIFY BELOW
#------------------------------------------------------------------------------

test:
	@$(MAKE) -sk test-all

test-all:	test-gitignore test-socket test-nmapit test-curlit

test-gitignore:
	@echo "nmapit" > .gitignore
	@echo "curlit" > .gitignore
	@echo "*.o" >> .gitignore
	@echo "*.sh" >> .gitignore
	@echo "*.unit" >> .gitignore

test-socket:	socket.unit
	@curl -sLO https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/static/txt/homework10/socket.unit.sh
	@chmod +x socket.unit.sh
	@./socket.unit.sh

socket.unit:	socket.unit.c socket.c
	$(CC) $(CFLAGS) -o $@ $^

test-nmapit:	nmapit
	@curl -sLO https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/static/txt/homework10/nmapit.test.sh
	@chmod +x nmapit.test.sh
	@./nmapit.test.sh

test-curlit:	curlit
	@curl -sLO https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/static/txt/homework10/curlit.test.sh
	@chmod +x curlit.test.sh
	@./curlit.test.sh

clean:
	@rm -f $(TARGETS) *.o *.sh *.unit
