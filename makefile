$(CC) = gcc
final: 
	$(CC) lazy.c parse.c -o final
Clean:
	rm *.o final
