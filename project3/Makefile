pc: pc.c
	gcc -Wall -Wextra -Werror -o pc pc.c eventbuf.c
pc.zip:
	rm -f $@
	zip $@ Makefile pc.c
