#!/bin/bash

# run with: . /[PATH_TO_THIS_DIR]/env.sh

read -p "Your Name? " DEBFULLNAME
read -p "Your Mail? " DEBMAIL
export DEBFULLNAME DEBMAIL

git config --global user.email "${DEBMAIL}"
git config --global user.name "${DEBFULLNAME}"
