testfs: testfs.o libvvsfs.a
	gcc $^ -o $@

testfs.o: testfs.c image.h block.h
	gcc -Wall -Wextra -c testfs.c

image.o: image.c
	gcc -Wall -Wextra -c image.c

block.o: block.c
	gcc -Wall -Wextra -c block.c

libvvsfs.a: image.o block.o
	ar rcs $@ $^

.PHONY: clean pristine
clean:
	rm -f *.o

pristine: clean
	rm -f testfs


