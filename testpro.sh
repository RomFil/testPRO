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
# Insert filepath to utils
export PATH=$PATH:$PWD/utils/mbrtu:$PWD/utils/readstr:$PWD/utils/sendstr;
# Loading parameters from MAIN.CONFIG
eval $(cat config/main.config)
if [ -z "$CONFIG" ]; then 
	echo "ERROR : parameter CONFIG does not exist"; 
	exit;
fi
if [ -z "$TARGET" ]; then 
	echo "ERROR : parameter TARGET does not exist"; 
	exit;
fi
if [ -z "$TEST" ]; then 
	echo "ERROR : parameter TEST does not exist"; 
	exit;
else
	if [ -n "$2" ]; then
		TEST=$2;
	fi
fi

# Loading parameters from CHANNELS.CONFIG
eval $(cat config/$CONFIG/main.config)
if [ -z "$FIRMWARE" ]; then 
	echo "ERROR : parameter FIRMWARE does not exist"; 
	exit;
fi
eval $(cat config/$CONFIG/$CHANNEL.config )
if [ -z "$ETHER" ]; then 
	echo "ERROR : parameter ETHER does not exist"; 
	exit;
fi
if [ -z "$PORT_CONSOLE" ]; then 
	echo "ERROR : parameter PORT_CONSOLE does not exist"; 
	exit;
fi
if [ -z "$PORT_MODBUS" ]; then 
	echo "ERROR : parameter PORT_MODBUS does not exist"; 
	exit;
fi
if [ -z "$UBOOT_IP_SERVER" ]; then 
	echo "ERROR : parameter UBOOT_IP_SERVER does not exist"; 
	exit;
fi
if [ -z "$UBOOT_IP_CLIENT" ]; then 
	echo "ERROR : parameter UBOOT_IP_CLIENT does not exist"; 
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
