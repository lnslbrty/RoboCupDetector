#!/bin/bash

if [ $(id -u) -ne 0 ]; then
	echo "$0: Dieses Skript kann nur als root ausgeführt werden."
	echo "Befehl: sudo $0"
	exit 1
fi

dpkg -i $(dirname $0)/dependencies/*.deb
apt-get install gcc autoconf cmake make cmake-curses-gui
apt-get install dh-make build-essential
apt-get install devscripts fakeroot debootstrap pbuilder
apt-get install libopencv-dev libmicrohttpd-dev
apt-get install doxygen graphviz
