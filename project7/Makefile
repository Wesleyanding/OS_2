testfs: testfs.o libvvsfs.a
	gcc $^ -o $@

testfs.o: testfs.c image.h block.h ctest.h free.h inode.h
	gcc -Wall -Wextra -c $<

image.o: image.c image.h
	gcc -Wall -Wextra -c $<

block.o: block.c block.h
	gcc -Wall -Wextra -c $<

free.o: free.c free.h
	gcc -Wall -Wextra -c $<

inode.o: inode.c inode.h
	gcc -Wall -Wextra -c $<

libvvsfs.a: image.o block.o free.o inode.o
	ar rcs $@ $^

.PHONY: clean pristine
clean:
	rm -f *.o

pristine: clean
	rm -f testfs

test: testfs
	./testfs


