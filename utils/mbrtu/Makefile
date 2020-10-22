CC:=gcc
LIBC += -I/usr/include/modbus -L/usr/lib -lmodbus

release: clean
	$(CC) $(CFLAGS) -Wall -o mbrtu -s mbrtu.c  $(LIBC)
clean:
	rm -rf mbrtu *~
