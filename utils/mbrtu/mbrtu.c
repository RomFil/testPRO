
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus/modbus-rtu.h>

#define OK				    0       /* OK */
#define ERR_INVALID_CMD     11		/* Invalid command */
#define ERR_INVALID_ARG     22		/* Invalid argument */
#define ERR_INVALID_MB      33		/* Invalid context modbus */

#define TR_SEC 			    1		/* Timeout 1 sec */
#define TR_USEC 		    5000	/* Timeout 5000 usec */

int simplified_out = 0;
int i;
char answer[2048]="";
char data[10];
uint16_t tab_reg[255];
int rc;

static void usage(const char *progname) {
	printf("Syntax: %s <device> <speed> <addr> <rh|ri|wh> <reg> [<data>] [<count>]\n", progname);
	printf("	-S or -s   - use simplified output (for script) \n");
	printf("	<device>   - /dev/ttyUSB0 \n");
	printf("	<speed>    - baudrate, only 9600,19200,57600,115200 \n");
	printf("	<addr> 	   - address device, 1...255 \n");
	printf("	<rh|ri|wh> - function modbus, rh-read holding ,ri-read input ,wh-write holding,\n");
    printf("	<reg>      - address register, 0...65535\n");
    printf("	[<data>]   - value for write (only wh), 0...65535\n");
    printf("	[<count>]  - amount registers for read (only rh,ri), 1...255\n");
    printf("Example: mbrtu -s /tty/USB0 19200 1 rh 10 10\n");
}

void print_data(int simple_out, int rc) {
	
	if (!simple_out) {
			strcat(answer, "{ \n\t\"data\": [\n");
			for (i = 0; i < rc; i++) {
				sprintf(data,"\t\t%d",tab_reg[i]);
				strcat(answer, data);
				if (i!=rc-1) {
					strcat(answer, ",\n");
				}
			}
			strcat(answer, "\n\t] \n}");
			printf("%s\n",answer);	
		} else {
			strcat(answer, "{ \"data\": [");
			for (i = 0; i < rc; i++) {
				sprintf(data,"%d",tab_reg[i]);
				strcat(answer, data);
				if (i!=rc-1) {
					strcat(answer, ",");
				}
			}
			strcat(answer, "] }");
			printf("%s\n",answer);	
		}
}

int main(int argc, char** argv) {
	
	char *prog_name;
	char port[32];
	int baudrate;
	int addr_dev;
	char *func;
	int reg;
	int data_count;
	modbus_t* ctx = NULL;
	
	prog_name=argv[0];
	
	if (argc > 1) {
		if (!strcmp("-S", argv[1]) || !strcmp("-s", argv[1])) {
			argc--;
			argv++;
			simplified_out = 1;
		}
	}
	
	if (argc < 7) {
		usage(prog_name);
		return(ERR_INVALID_CMD);
	}
	
	if (*argv[1] == '/') {
		snprintf(port, sizeof(port), "%s", argv[1]);
	}

	baudrate = atoi(argv[2]);
	if (!(baudrate==9600 || baudrate==19200 || baudrate==57600 || baudrate==115200)) {
		usage(prog_name);
		return(ERR_INVALID_ARG);
	}
	
	addr_dev = atoi(argv[3]);
	if (!(addr_dev>=1 && addr_dev<=255)) {
		usage(prog_name);
		return(ERR_INVALID_ARG);
	}
	
	func = argv[4];
	if (strcmp(func,"rh") && strcmp(func,"ri") && strcmp(func,"wh")) {
		usage(prog_name);
		return(ERR_INVALID_ARG);
	}
	
	reg = atoi(argv[5]);
	if (!(reg>=0 && reg<=65535)) {
		usage(prog_name);
		return(ERR_INVALID_ARG);
	}
	
	data_count = atoi(argv[6]);
	if (!strcmp(func,"rh") && !strcmp(func,"ri"))
	{
		if (!(data_count>=1 && data_count<=255)) {
			usage(prog_name);
			return(ERR_INVALID_ARG);
		}
	} else {
		if (!(data_count>=0 && data_count<=65535)) {
			usage(prog_name);
			return(ERR_INVALID_ARG);
		}
	}
	
	ctx = modbus_new_rtu(port, baudrate, 'N', 8, 1);
    
    if (!ctx) {
		modbus_free(ctx);
		return(ERR_INVALID_MB);
	} 
		
	modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS232);
	modbus_set_slave(ctx, addr_dev);
	modbus_set_response_timeout(ctx, TR_SEC, TR_USEC); 
	//modbus_set_recovery_mode(ctx, MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);
		
	rc = modbus_connect(ctx);
	if (rc == -1) {
		fprintf(stderr, "ERROR : %s \n", modbus_strerror(errno));
		modbus_free(ctx);
		return(ERR_INVALID_MB);
	}
			
	// ----- polling holding registers
	if (!strcmp(func,"rh")) {
		rc = modbus_read_registers(ctx, reg, data_count, tab_reg);
		if (rc == -1) {
			fprintf(stderr, "ERROR : %s (%d)\n", modbus_strerror(errno), errno);
			return(errno);
		} 
		print_data(simplified_out, rc);
	}
			
	// ----- polling input registers
	if (!strcmp(func,"ri")) {
		rc = modbus_read_input_registers(ctx, reg, data_count, tab_reg);
		if (rc == -1) {
			fprintf(stderr, "ERROR : %s (%d)\n", modbus_strerror(errno), errno);
			return(errno);
		} 
		print_data(simplified_out, rc);
	}
			
	// ----- writing holding registers
	if (!strcmp(func,"wh")) {
		rc = modbus_write_register(ctx, reg, data_count);
		if (rc == -1) {
			fprintf(stderr, "ERROR : %s (%d)\n", modbus_strerror(errno), errno);
			return(errno);
		} 
    }
			
    modbus_close(ctx);
	modbus_free(ctx);

	return(0);

}
