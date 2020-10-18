CC:=gcc
LIBC += -I/usr/include/modbus -L/usr/lib -lmodbus

release: clean
	$(CC) $(CFLAGS) -Wall -o mbrtu -s mbrtu.c  $(LIBC)
	$(CC) $(CFLAGS) -Wall -o readstr -s readstr.c   
	$(CC) $(CFLAGS) -Wall -o sendstr -s sendstr.c   
clean:
	rm -rf mbrtu readstr sendstr *~
