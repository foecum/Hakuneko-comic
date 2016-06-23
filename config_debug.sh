#!/bin/bash

# import setings from config-default
. ./config_default.sh

# overwrite settings from config-default
PKGNAME="hakuneko-dbg"

CFLAGS="
    -g
    -c
    -Wall
    $(wx-config --version=3.0 --static=no --debug=yes --cflags)
    "

LDFLAGS=""
LDLIBS="
    $(wx-config --version=3.0 --static=no --debug=yes --libs --gl-libs)
    -lcurl
    -lcrypto
    "
