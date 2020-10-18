// C library headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Linux headers
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <sys/file.h>

#define OK				    0       /* OK */
#define ERR				    1       /* ERROR */
#define ERR_INVALID_CMD     11		/* Invalid command */
#define ERR_IO			    10		/* Invalid input/output */

#define UART_FAILURE 		-1
#define UART_SUCCESS 		0

#define BUFF_SIZE			30000

char read_buffer[BUFF_SIZE] = { 0 };
struct termios save_tty = { 0 };

static int set_attr_tty(int fd, int baudrate, int parity, int bits, int stopbits) {
	struct termios tty;
	
	if (tcgetattr(fd, &save_tty) < 0) 
		return 1;
	
	if (tcgetattr(fd, &tty) < 0) 
		return 1;
		
	cfsetispeed(&tty, baudrate);
	cfsetospeed(&tty, baudrate);

	tty.c_iflag &= ~(IGNCR | ICRNL | IUCLC | INPCK | IXON | IXANY | IGNPAR);
	tty.c_oflag &= ~(OPOST | OLCUC | OCRNL | ONLCR | ONLRET);
	tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHONL);
	tty.c_cflag &= ~(CBAUD | CSIZE | CSTOPB | CLOCAL | PARENB);
	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VEOF] = 1;
	tty.c_cflag |= (baudrate | bits | CREAD | parity | stopbits);
	
	if (tcsetattr(fd, TCSANOW, &tty) < 0)
		return 1;
		
	return 0;		
}

int waitquiet(int fd, int timeout_ms) {
	fd_set rfds;
	char cb[1];
	int len;
	struct timeval tv;

	while(1) {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		tv.tv_sec  = timeout_ms / 1000;
		tv.tv_usec = (timeout_ms % 1000) * 1000;
			
		switch(select(fd + 1, &rfds, NULL, NULL, &tv)) {
			case -1:
					return -1;
			case 0:
					return 0;
			default:	
					if (FD_ISSET(fd, &rfds)) {
						len = read(fd, cb, 1);
						if (!len)
							return -1;
					}
					else {
						return -1;
					}
		}
	}
}

int uart_find_string(int fd, char *buf, unsigned int size_buf, const char *strfind, unsigned int timeout_ms) {

	fd_set rfds;
	int cnt;
	struct timeval tv;
	int i;
	int N;
	char fl;

	tv.tv_sec  = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;
	cnt = 0;
	N = strlen(strfind);

	while(1) {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		switch(select(fd + 1, &rfds, NULL, NULL, &tv)) {
			case -1:
				return -1;
			case 0:
				buf[cnt] = 0; 
				return -1;
			default:	
				if (FD_ISSET(fd, &rfds)) {
					while (read(fd, buf + cnt, 1) == 1) {					
						
						if (buf[cnt]=='\0') {
							buf[cnt]='\r';
							continue;
						}
						
						fl=0;
						for (i = 0; i < N-1; i++) {
							if (buf[cnt-i] == strfind[N-i-1]) {
								fl=1;
							} else {
								fl=0;
								break;
							}
						}
						
						if (fl)
						{
							buf[++cnt] = 0;
							return cnt;
						}
						cnt++;
						if (cnt == size_buf) {
							buf[cnt] = 0;
							return 0;	
						}
					}
				}
				else {
					return -1;
				}
		}
	}
	return -1;
}

static void usage(const char *progname) {
	printf("Syntax: %s <device> \"<string>\" <timeout>\n", progname);
	printf("	<device>   - /dev/ttyUSB0 \n");
	printf("   \"<string>\" - string for find uart\n");
	printf("	<timeout>  - timeout uart \n");
    printf("Example: %s -/tty/USB0 \"default:\" 2000\n", progname);
}

int main(int argc, char** argv) {
	
	int rc;
	int fd;
	char *prog_name;
	char port[32];
	char *string_find;
	unsigned int timeout;
	char welcome = 0;

	prog_name=argv[0];
	
	if (argc > 1) {
		if (!strcmp("-wu", argv[1])) {
			argc--;
			argv++;
			welcome = 1;
		}
	}
	
	if (argc < 3) {
		usage(prog_name);
		fprintf(stderr, "ERROR (few parametrs command) : %s (%d)\n", strerror(errno), errno);
		return(ERR);
	}
	
	if (*argv[1] == '/') {
		snprintf(port, sizeof(port), "%s", argv[1]);
	}
	
	string_find = argv[2];
	if (strlen(string_find) <= 0) {
		fprintf(stderr, "ERROR (check length find_string) : %s (%d)\n", strerror(errno), errno);
		return(ERR);
	}
	
	timeout = atoi(argv[3]);
	
	// ----- Open port tty
	fd = open(port, O_RDWR | O_NONBLOCK | O_NOCTTY);
	if (fd == -1) {
		fprintf(stderr, "ERROR (open uart) : %s (%d)\n", strerror(errno), errno);
		return(ERR_IO);
	}

	flock(fd, LOCK_EX);	
	
	if (set_attr_tty(fd, B57600, 0 , CS8, 0)) {
		fprintf(stderr, "ERROR (set attr uart) : %s (%d)\n", strerror(errno), errno);
		return(ERR_IO);
	}
	/*
	if (waitquiet(fd, 15)) {
		fprintf(stderr, "ERROR (error flush uart) : %s (%d)\n", strerror(errno), errno);
		return(ERR_IO);
	}
	*/
	rc = uart_find_string(fd, read_buffer, BUFF_SIZE, string_find,  timeout); 
	if(rc < 0) {
		fprintf(stderr, "ERROR (read uart string) : timeout\n");
		return(ERR);
	} 
	
	//printf("READ_BUFFER ----------- %s\n", read_buffer);
	
	// ----- Print UBOOT welcome
	if (welcome) {
		char *istr;
		int cnt;
		char uboot_str[256] = { 0 };
		
		//printf("read_buffer --- %s\n", read_buffer);
		istr = strstr(read_buffer, "U-Boot");
		if (!istr) {
			fprintf(stderr, "ERROR (find U-Boot)\n");
			return(ERR);
		}
		//printf("istr --- %s\n", istr);
		cnt = 0;
		while (*istr) {
			if (istr[0] == '\n') {
				break;
			}
			uboot_str[cnt] = istr[0];
			cnt++;
			istr++;
		}
		printf("%s\n", uboot_str);
	}
	
	if (tcsetattr(fd, TCSANOW, &save_tty) < 0) {
		fprintf(stderr, "ERROR (restore attr uart)\n");
		return(ERR);
	}

	flock(fd, LOCK_UN);
    if (fd) {
		close(fd);
	} else {
		fprintf(stderr, "ERROR (closing uart)\n");
		return(ERR);
	}
	
	return(OK);
}
