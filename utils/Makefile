CC:=gcc
LIBC += -I/usr/include/modbus -L/usr/lib -lmodbus

release: clean
	$(CC) $(CFLAGS) -Wall -o mbrtu/mbrtu -s mbrtu/mbrtu.c  $(LIBC)
	$(CC) $(CFLAGS) -Wall -o readstr/readstr -s readstr/readstr.c   
	$(CC) $(CFLAGS) -Wall -o sendstr/sendstr -s sendstr/sendstr.c   
clean:
	rm -rf mbrtu/mbrtu readstr/readstr sendstr/sendstr *~
