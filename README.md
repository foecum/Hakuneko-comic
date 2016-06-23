# Hakuneko-comic
This is a fork of the [Hakuneko manga downloader](https://sourceforge.net/projects/hakuneko/)

This is an attempt at making a comic book version of this app for Unix systems

### Installation
Clone the repo

#### LINUX (DEB)

##### requirements to build hakuneko on debian

    $ bash
    $ make
    $ build-essentials (gcc, g++, lib*,...)
      # OR: clang, make, lib*,...
    $ libwxgtk3.0-dev
    $ libwxbase3.0-dev
    $ libcurl4-openssl-dev
    $ libssl-dev

##### build & install hakuneko 

    # open a terminal
    # change into hakuneko directory
    # run configure (with --prefix=DIR if you want to install in a directory other than /usr/local)
    $ ./configure --prefix=/usr
    # OR (if you want to use clang instead of gcc):
    $ ./configure --config-clang --prefix=/usr
    $ make
    # run as root or installation will fail
    $ sudo make install

    # remove hakuneko
    # run as root or uninstallation will fail
    $ sudo make uninstall

    # NOTE: create a source package for redistribution
    $ make tgz

    # NOTE: create a debian binary package for redistribution
    # (configure with prefix /usr to prevent lintian errors)
    # (run as root for correct ownership of files)
    $ ./configure --prefix=/usr
    # OR (if you want to use clang instead of gcc):
    $ ./configure --config-clang --prefix=/usr
    $ sudo make deb


#### LINUX (RPM)

##### similar to the DEB variant 

    $ ./configure --prefix=/usr
    $ sudo make rpm


#### WINNT 

##### Requirements to compile HakuNeko on win32 

    1. MinGW32 (GCC >= 4.5)

##### Compiling HakuNeko 

    # open a command prompt (cmd)
    # change into hakuneko directory
    # run the following commands
    $ ./configure --config-mingw32
    # or
    $ ./configure --config-mingw32-portable
    $ make
    # binaries can than be found in hakuneko/build/msw/...

