#!/bin/bash

# run with: . /[PATH_TO_THIS_DIR]/env.sh

# check for dpkg/dh enviroment variables
if [ "x${DEBFULLNAME}" = "x" ]; then
	read -p "Your Name? " DEBFULLNAME
	echo "export DEBFULLNAME=${DEBFULLNAME}" >> "${HOME}/.profile"
fi

if [ "x${DEBMAIL}" = "x" ]; then
	read -p "Your Mail? " DEBMAIL
	echo "export DEBMAIL=${DEBMAIL}" >> "${HOME}/.profile"
fi

export DEBFULLNAME DEBMAIL


git config --global user.email "${DEBMAIL}"
git config --global user.name "${DEBFULLNAME}"
