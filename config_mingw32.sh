#!/bin/bash

# import setings from config-default
. ./config_default.sh

# overwrite settings from config-default
RCPATTERN="resource.rc"
RCDIR="res"
DISTROOT="build/msw"
BINFILE="$DISTROOT/bin/$PKGNAME.exe"

CFLAGS="
    -c
    -Wall
    -O2
    -D__GNUWIN32__
    -D__WXMSW__
    -DwxUSE_UNICODE
    -DCURL_STATICLIB
    -Iinclude/msw
    -Ilib/msw/32/wx/mswu
    "

RC="windres.exe"
RCFLAGS="
    -J rc
    -O coff
    -F pe-i386
    -Iinclude/msw
    "

LDFLAGS="
    -s
    -static
    -static-libgcc
    -static-libstdc++
    -mwindows
    "
LDLIBS="
    -Llib/msw/32/wx
    -lwxmsw30u
    -lwxmsw30u_gl
    -lwxexpat
    -lwxregexu
    -lwxpng
    -lwxjpeg
    -lwxtiff
    -lwxzlib
    -Llib/msw/32/curl
    -lcurl -lrtmp -lwinmm -lssh2 -lssl -lcrypto -lgdi32 -lcrypt32 -lz -lidn -lwldap32 -lws2_32
    -Llib/msw/32/openssl
    -lcrypto
    -L/mingw/lib
    -lwinspool
    -lole32
    -loleaut32
    -luuid
    -lcomctl32
    -lwsock32
    "
