CC=		gcc
CFLAGS=		-Wall -g -std=gnu99
LD=		gcc
LDFLAGS=	-L.
TARGETS=	findit

all:		$(TARGETS)

#-------------------------------------------------------------------------------
# TODO: Add rules for object files
#-------------------------------------------------------------------------------

list.o: list.c findit.h
	$(CC) $(CFLAGS) -c -o $@ $<

filter.o: filter.c findit.h
	$(CC) $(CFLAGS) -c -o $@ $<

findit.o: findit.c findit.h
	$(CC) $(CFLAGS) -c -o $@ $<

#-------------------------------------------------------------------------------
# TODO: Add rules for executables
#-------------------------------------------------------------------------------

findit: findit.o filter.o list.o
	$(LD) $(LDFLAGS) -o $@ $^

#-------------------------------------------------------------------------------
# DO NOT MODIFY BELOW
#-------------------------------------------------------------------------------

test:
	@$(MAKE) -sk test-all

test-all:	test-gitignore test-list test-filter test-findit

test-gitignore:
	@echo "findit" > .gitignore
	@echo "*.o" >> .gitignore
	@echo "*.sh" >> .gitignore
	@echo "*.unit" >> .gitignore

test-list:	list.unit
	@curl -sLO https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/static/txt/homework08/list.unit.sh
	@chmod +x list.unit.sh
	@./list.unit.sh

list.unit.o:	list.unit.c findit.h
	@$(CC) $(CFLAGS) -c -o $@ $<

list.unit:	list.unit.o list.o
	@$(LD) $(LDFLAGS) -o $@ $^

test-filter:	filter.unit
	@curl -sLO https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/static/txt/homework08/filter.unit.sh
	@chmod +x filter.unit.sh
	@./filter.unit.sh

filter.unit.o:	filter.unit.c findit.h
	@$(CC) $(CFLAGS) -c -o $@ $<

filter.unit:	filter.unit.o filter.o
	@$(LD) $(LDFLAGS) -o $@ $^

test-findit:	findit
	@curl -sLO https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/static/txt/homework08/findit.test.sh
	@chmod +x findit.test.sh
	@./findit.test.sh

clean:
	@rm -f *.o *.sh *.unit findit
