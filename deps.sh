#!/bin/bash

if [ $(id -u) -ne 0 ]; then
	echo "$0: Dieses Skript kann nur als root ausgeführt werden."
	echo "Befehl: sudo $0"
	exit 1
fi

# Raspberrypi Pakete installieren (Bitte nur diese nutzen!)
dpkg -i $(dirname $0)/dependencies/*.deb
# Build Enviroment
apt-get install gcc autoconf cmake make cmake-curses-gui
apt-get install dh-make build-essential
apt-get install devscripts fakeroot debootstrap pbuilder
# OpenCV Bibliotheken + Header
apt-get install libopencv-dev libmicrohttpd-dev
# doxygen + UML Generator
apt-get install doxygen graphviz
