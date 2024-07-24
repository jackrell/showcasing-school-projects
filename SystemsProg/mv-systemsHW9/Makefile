CC=		gcc
CFLAGS=		-Wall -g -std=gnu99
TARGETS=	moveit timeit

all:		$(TARGETS)

#------------------------------------------------------------------------------
# TODO: Rules for moveit, timeit
#------------------------------------------------------------------------------

moveit: moveit.c
	$(CC) $(CFLAGS) -o $@ $^

timeit: timeit.c
	$(CC) $(CFLAGS) -o $@ $^

#------------------------------------------------------------------------------
# DO NOT MODIFY BELOW
#------------------------------------------------------------------------------

test:
	@$(MAKE) -sk test-all

test-all:	test-gitignore test-moveit test-timeit

test-gitignore:
	@echo "moveit" > .gitignore
	@echo "timeit" > .gitignore
	@echo "*.sh" >> .gitignore

test-moveit:	moveit
	@curl -sLO https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/static/txt/homework09/moveit.test.sh
	@chmod +x moveit.test.sh
	@./moveit.test.sh

test-timeit:	timeit
	@curl -sLO https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/static/txt/homework09/timeit.test.sh
	@chmod +x timeit.test.sh
	@./timeit.test.sh

clean:
	@rm -f $(TARGETS) *.sh
