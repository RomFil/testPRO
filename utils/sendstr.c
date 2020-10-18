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

static void usage(const char *progname) {
	printf("Syntax: %s <device> \"<command>\"\n", progname);
	printf("	<device>   - /dev/ttyUSB0 \n");
	printf("   \"<command>\" - command to uart\n");
    printf("Example: %s -/tty/USB0 \"Y\"\n", progname);
}

int uart_send(int fd, char *cmd) {
	int cnt;
	int len;
	
	len = strlen(cmd);
	cnt = len;
	if (len) {
		cnt = write(fd, cmd, len);
	}
	
	return (len != cnt);
}

int main(int argc, char** argv) {
	
	int rc;
	int fd;
	char *prog_name;
	char port[32];
	char *cmd_str;

	prog_name=argv[0];
	
	if (argc < 2) {
		usage(prog_name);
		fprintf(stderr, "ERROR (few parametrs command) : %s (%d)\n", strerror(errno), errno);
		return(ERR);
	}
	
	if (*argv[1] == '/') {
		snprintf(port, sizeof(port), "%s", argv[1]);
	}
	
	cmd_str = argv[2];
	if (strlen(cmd_str) <= 0) {
		fprintf(stderr, "ERROR (check length command) : %s (%d)\n", strerror(errno), errno);
		return(ERR);
	}
	
	if (!strcmp("-n", cmd_str))
	{
		cmd_str = "\n";
	}
	
	if (!strcmp("-u", cmd_str))
	{
		cmd_str = "\025";
	}
	
	//printf("cmd= %s\n",cmd_str);
	
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
	
	rc = uart_send(fd, cmd_str); 
	if(rc < 0) {
		fprintf(stderr, "ERROR (send uart command) : timeout\n");
		return(ERR);
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
	
	usleep(50000);
	
	return(OK);
}
