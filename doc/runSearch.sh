#!/bin/sh

export $(dbus-launch)
source /etc/search.conf
cd ${BINDIR}
${BIN}
