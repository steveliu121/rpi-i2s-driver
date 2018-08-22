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

MOD="alc5680-voicecard"
VER="0.1"

echo "------------------------------------------------------"
echo "remove kernel modules"
echo "------------------------------------------------------"
dkms remove --force -m $MOD -v $VER --all

echo "------------------------------------------------------"
echo "remove dkms"
echo "------------------------------------------------------"
rm  -rf /var/lib/dkms/$MOD

echo "------------------------------------------------------"
echo "remove src copy"
echo "------------------------------------------------------"
rm  -rf /usr/src/$MOD-$VER

echo "------------------------------------------------------"
echo "unset kernel modules"
echo "------------------------------------------------------"
sed -i '/snd-soc-rt5677/d' /etc/modules
sed -i '/snd-soc-rpi-rt5679-machine/d' /etc/modules

echo "------------------------------------------------------"
echo "remove dtbos"
echo "------------------------------------------------------"
rm  /boot/overlays/rpi-rt5679-machine.dtbo

echo "------------------------------------------------------"
echo "unset droverlays"
echo "------------------------------------------------------"
sed -i 's/dtparam=i2s=on/#dtparam=i2s=on/g' /boot/config.txt
sed -i '/dtoverlay=rpi-rt5679-machine/d' /boot/config.txt

echo "------------------------------------------------------"
echo "Please reboot your raspberry pi to apply all settings"
echo "------------------------------------------------------"
