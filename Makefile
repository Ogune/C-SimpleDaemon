all: deamon

deamon: *.c
	cc -Werror -Wextra *.c -o deamon
clean:
	rm -rf deamon

re: clean all
