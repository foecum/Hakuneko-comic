#!/bin/bash

# import setings from config-default
. ./config_mingw32.sh

# overwrite settings from config-default
BINFILE="$DISTROOT/bin/$PKGNAME-portable.exe"

CFLAGS="
    -c
    -Wall
    -O2
    -DPORTABLE
    -D__GNUWIN32__
    -D__WXMSW__
    -DwxUSE_UNICODE
    -DCURL_STATICLIB
    -Iinclude/msw
    -Ilib/msw/32/wx/mswu
    "
