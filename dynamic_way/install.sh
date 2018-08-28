#!/bin/bash

if [[ $EUID -ne 0 ]]; then
	echo "This script must be run as root (use sudo)" 1>&2
	exit 1
fi

is_Raspberry=$(cat /proc/device-tree/model | awk  '{print $1}')
if [ "x${is_Raspberry}" != "xRaspberry" ] ; then
	echo "Sorry, this drivers only works on raspberry pi"
	exit 1
fi

VER="0.1"

apt update
apt-get -y install raspberrypi-kernel-headers raspberrypi-kernel
apt-get -y install dkms libasound2-plugins

echo "------------------------------------------------------"
echo "build && install modules for the currently running kernel and arch .."
echo "------------------------------------------------------"
SRC_DIR="./src"
DRV_SRC="$SRC_DIR/drv"
DTB_SRC="$SRC_DIR/dtb"

function install_module {
	SRC=$1
	MOD=$2

	echo "------------------------------------------------------"
	echo "build&&install kernel modules"
	echo "------------------------------------------------------"

	if [[ -e /usr/src/$MOD-$VER || -e /var/lib/dkms/$MOD/$VER ]]; then
		dkms remove --force -m $MOD -v $VER --all
		rm -fr /var/lib/dkms/$MOD
		rm -rf /usr/src/$MOD-$VER
	fi

	mkdir -p /usr/src/$MOD-$VER
	cp -a $SRC/* /usr/src/$MOD-$VER
	dkms add -m $MOD -v $VER
	dkms build -m $MOD -v $VER && dkms install --force -m $MOD -v $VER
}

install_module $DRV_SRC "alc5680-voicecard"

echo "------------------------------------------------------"
echo "set kernel moduels"
echo "------------------------------------------------------"
grep -q "snd-soc-rt5677" /etc/modules || \
	echo "snd-soc-rt5677" >> /etc/modules
grep -q "snd-soc-rpi-rt5677-machine" /etc/modules || \
	echo "snd-soc-rpi-rt5677-machine" >> /etc/modules

echo "------------------------------------------------------"
echo "install dtbos"
echo "------------------------------------------------------"
cp $DTB_SRC/rpi-rt5677-machine.dtbo /boot/overlays

echo "------------------------------------------------------"
echo "set dtoverlays"
echo "------------------------------------------------------"
sed -i 's/#dtparam=i2s=on/dtparam=i2s=on/g' /boot/config.txt
grep -q "dtoverlay=rpi-rt5677-machine" /boot/config.txt || \
	echo "dtoverlay=rpi-rt5677-machine" >> /boot/config.txt

echo "------------------------------------------------------"
echo "Please reboot your raspberry pi to apply all settings"
echo "------------------------------------------------------"
