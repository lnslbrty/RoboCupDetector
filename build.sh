#!/bin/bash

DIR=$(dirname $0)

sudo $DIR/deps.sh

OLDPWD=$(pwd)

cd ${DIR}/raspicam
dpkg-buildpackage -b -us -uc
cd ${OLDPWD}

cmake . && make

sudo dpkg -i *.deb
