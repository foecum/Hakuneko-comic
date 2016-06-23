#!/bin/bash

PKGNAME="hakuneko"
PKGVERSION="1.4.1"
PKGSECTION="net"
PKGAUTHOR="Ronny Wegener <wegener.ronny@gmail.com>"
PKGHOMEPAGE="http://hakuneko.sourceforge.net"
PKGDEPENDS=""
PKGDESCRIPTION="Manga Downloader based on wxGTK
 HakuNeko allows you to download manga images from
 some selected online manga reader websites
 .
 Currently Supported:
 .
 * Batoto
 * DynastyScans
 * KissAnime
 * KissManga
 * MangaFox
 * Mangago
 * MangaHead
 * MangaHere
 * MangaHere (es)
 * MangaPanda
 * MangaReader"

SRCPATTERN="*.cpp"
SRCDIR="src"
RCPATTERN=""
RCDIR=""
OBJDIR="obj"
DISTROOT="build/linux"
BINFILE="$DISTROOT/bin/$PKGNAME"

CC="g++"
CFLAGS="
    -c
    -Wall
    -O2
    $(wx-config --version=3.0 --static=no --debug=no --cflags)
    "

RC=""
RCFLAGS=""

LD="g++"
LDFLAGS="-s"
LDLIBS="
    $(wx-config --version=3.0 --static=no --debug=no --libs)
    -lcurl
    -lcrypto
    "

echo "#define VERSION wxT(\"$PKGVERSION\")" > "src/version.h"
echo "#define COPYRIGHT wxT(\"(C) $(date +%Y)\")" >> "src/version.h"
