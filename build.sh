#!/bin/bash
set -e

if [ $(id -u) -eq 0 ]; then
	echo "$0: Dieses Skript sollte _nicht_ als root ausgeführt werden."
	exit 1
fi

DIR=$(realpath $(dirname $0))
echo "* ${DIR}"

sudo $DIR/deps.sh

OLDPWD=$(pwd)

cd ${DIR}/raspicam
dpkg-buildpackage -b -us -uc
cd ${DIR}
sudo dpkg -i ${DIR}/raspicam*.deb
dpkg-buildpackage -b -us -uc
sudo dpkg -i ${DIR}/../bsz7robocup_0.1_armhf.deb ${DIR}/../bsz7robocup*.deb

cd ${OLDPWD}
echo "* robocup kann nun ausgeführt werden (Tipp: `robocup -p`)"
