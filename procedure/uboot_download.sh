#!/bin/bash

utils/readstr -wu $PORT_CONSOLE "default: 3" 5000
if [ $? -ne 0 ]; then
	echo "ERROR WELCOME UBOOT"
	exit
fi
	
utils/sendstr $PORT_CONSOLE 2	
utils/readstr $PORT_CONSOLE "(Y/N)" 2000
if [ $? -ne 0 ]; then
	echo "ERROR START UBOOT"
	exit
fi
	
utils/sendstr $PORT_CONSOLE "y"
utils/readstr $PORT_CONSOLE "==:" 2000
if [ $? -ne 0 ]; then
	echo "ERROR CONFIRM UBOOT"
	exit
fi

utils/sendstr $PORT_CONSOLE -u
utils/sendstr $PORT_CONSOLE "$UBOOT_IP_CLIENT"
utils/sendstr $PORT_CONSOLE -n
utils/readstr $PORT_CONSOLE "==:" 2000
if [ $? -ne 0 ]; then
	echo "ERROR IP CLIENT"
	exit
fi
	
utils/sendstr $PORT_CONSOLE -u
utils/sendstr $PORT_CONSOLE "$UBOOT_IP_SERVER"
utils/sendstr $PORT_CONSOLE -n
utils/readstr $PORT_CONSOLE "==:" 2000
if [ $? -ne 0 ]; then
	echo "ERROR IP SERVER"
	exit
fi

utils/sendstr $PORT_CONSOLE "$FIRMWARE"
utils/sendstr $PORT_CONSOLE -n
	
utils/readstr $PORT_CONSOLE "Loading" 3000

if [ $? -ne 0 ]; then
	echo "ERROR START DOWNLOAD"
	exit
fi
	
echo "Starting download firmware $FIRMWARE"
	
# Starting TFTP server
$(sudo ip addr add $UBOOT_IP_SERVER/24 dev $ETHER)
#sudo service networking restart
sudo systemctl start tftpd-hpa.service
	
# Checking loading device
utils/readstr $PORT_CONSOLE "done" 40000
if [ $? -ne 0 ]; then
	echo "ERROR СOMPLETE DOWNLOAD"
	exit
fi
	
# Stoping TFTP server
sudo systemctl stop tftpd-hpa.service
$(sudo ip addr del $UBOOT_IP_SERVER/24 dev $ETHER)
	
echo "OK"	
echo "Starting reload firmware $FIRMWARE"

# Checking start device
utils/readstr $PORT_CONSOLE "Starting kernel" 90000
if [ $? -ne 0 ]; then
	echo "ERROR СOMPLETE DOWNLOAD"
	exit
fi
	
echo "OK"

