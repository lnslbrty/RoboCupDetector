#!/bin/bash

DIR=$(dirname $0)
echo "* ${DIR}"

#. ${DIR}/env.sh
sudo $DIR/deps.sh

OLDPWD=$(pwd)

cd ${DIR}/raspicam
dpkg-buildpackage -b -us -uc
cd ${OLDPWD}
