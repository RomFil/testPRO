#!/bin/bash

# Getting command script
if [ -n "$1" ];
then
	CHANNEL=$1;
else
	echo "ERROR : no parameters for testpro";
	echo "Syntax: testpro <channel> <procedure_name>";
	echo "	<channel> - 0 ... 3 number channel";
	echo "	<procedure_name> - name testing procedure";
	echo "Example: testpro 0 test";
	exit;
fi
# Checking range CHANNEL
if [ "$CHANNEL" -lt "0" -o "$CHANNEL" -gt "3" ]; then 
	echo "ERROR : variable CHANNEL is out of range"; 
	exit;
fi

# Loading parameter CONFIG from file
CONFIG=$(grep -i "CONFIG=" config/main.config | cut -d '=' -f2)
if [ -z "$CONFIG" ]; then 
	echo "ERROR : parameter CONFIG does not exist"; 
	exit;
fi
# Loading parameter TARGET from file
TARGET=$(grep -i "TARGET=" config/main.config | cut -d '=' -f2)
if [ -z "$TARGET" ]; then 
	echo "ERROR : parameter TARGET does not exist"; 
	exit;
fi
# Loading parameter TEST from command string or file 
if [ -n "$2" ]; then
	TEST=$2;
else
	TEST=$(grep -i "TEST=" config/main.config| cut -d '=' -f2)
fi
if [ -z "$TEST" ]; then 
	echo "ERROR : parameter TEST does not exist"; 
	exit;
fi
# Loading parameter FIRMWARE from file 
FIRMWARE=$(grep -i "FIRMWARE=" config/$CONFIG/main.config | cut -d '=' -f2)
if [ -z "$FIRMWARE" ]; then 
	echo "ERROR : parameter FIRMWARE does not exist"; 
	exit;
fi
# Loading parameter UBOOT_IP_SERVER from file 
UBOOT_IP_SERVER=$(grep -i "UBOOT_IP_SERVER=" config/$CONFIG/main.config | cut -d '=' -f2)
if [ -z "$UBOOT_IP_SERVER" ]; 
then 
	echo "ERROR : parameter UBOOT_IP_SERVER does not exist"; 
	exit;
fi
# Checking posibility IP address server for current PC 
#if [ -z "$(ip a | grep inet | grep "$UBOOT_IP_SERVER")" ]; then
#	echo "ERROR : this PC cannot run TFTP at IP address $UBOOT_IP_SERVER"
#	exit
#fi
# Loading parameter UBOOT_IP_CLIENT from file 
UBOOT_IP_CLIENT=$(grep -i "UBOOT_IP_CLIENT=" config/$CONFIG/main.config | cut -d '=' -f2)
if [ -z "$UBOOT_IP_CLIENT" ]; then 
	echo "ERROR : parameter UBOOT_IP_CLIENT does not exist"; 
	exit;
fi
# Loading parameter TFTP_DELAY from file
# ?????????????????????????????
#TFTP_DELAY=$(grep -i "TFTP_DELAY=" config/$CONFIG/main.config | cut -d '=' -f2)
#if [ -z "$TFTP_DELAY" ]; then 
#	echo "ERROR : parameter TFTP_DELAY does not exist"; 
#	exit;
#fi
# Loading parameter ETHER from file 
ETHER=$(grep -i "ETHER=" config/$CONFIG/$CHANNEL.config | cut -d '=' -f2)
if [ -z "$ETHER" ]; then 
	echo "ERROR : parameter ETHER does not exist"; 
	exit;
fi
# Loading parameter PORT_CONSOLE from file
PORT_CONSOLE=$(grep -i "PORT_CONSOLE=" config/$CONFIG/$CHANNEL.config | cut -d '=' -f2)
if [ -z "$PORT_CONSOLE" ]; then 
	echo "ERROR : parameter PORT_CONSOLE does not exist"; 
	exit;
fi
# Loading parameter PORT_MODBUS from file
PORT_MODBUS=$(grep -i "PORT_MODBUS=" config/$CONFIG/$CHANNEL.config | cut -d '=' -f2)
if [ -z "$PORT_MODBUS" ]; then 
	echo "ERROR : parameter PORT_MODBUS does not exist"; 
	exit;
fi
# Printing welcome string
echo "testpro $commit , target: $TARGET , test: $TEST";
# Find testing program
if [ -f !"test/$TARGET/$TEST" ]; then 
	echo "ERROR : can't find testing program!"; 
	exit;
fi
# Execute testing program
if [ "$TEST" == "uboot_download" ]; 
then 
	
	source procedure/uboot_download.sh

fi

#set -x

#set +x
