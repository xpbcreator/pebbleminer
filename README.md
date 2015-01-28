Pebbleminer
==============

This is a multi-threaded CPU miner for Boulderhash (used by Pebblecoin), fork of [lucasjones](https://github.com/lucasjones)'s [CPUMiner-multi](https://github.com/lucasjones/cpuminer-multi).

#### Table of contents

* [Algorithms](#algorithms)
* [Dependencies](#dependencies)
* [Download](#download)
* [Build](#build)
* [Usage instructions](#usage-instructions)
* [Donations](#donations)
* [Credits](#credits)
* [License](#license)

Algorithms
==========
#### Currently supported
 * ✓ __boulderhash2__ (Pebblecoin)

Dependencies
============
* libcurl			http://curl.haxx.se/libcurl/
* openssl     https://www.openssl.org/

Download
========
* Binary releases: N/A
* Git tree:   https://github.com/xpbcreator/pebbleminer
* Clone with `git clone https://github.com/xpbcreator/pebbleminer`

Build
=====

#### Basic *nix build instructions:
 * ./autogen.sh	# only needed if building from git repo
 * ./nomacro.pl	# only needed if building on Mac OS X or with Clang
 * ./configure CFLAGS="*-march=native*"
   * # Use -march=native if building for a single machine
 * make

#### Notes for AIX users:
 * To build a 64-bit binary, export OBJECT_MODE=64
 * GNU-style long options are not supported, but are accessible via configuration file

#### Basic Windows build instructions, using MinGW:
 * Install MinGW and the MSYS Developer Tool Kit (http://www.mingw.org/)
   * Make sure you have mstcpip.h in MinGW\include
 * If using MinGW-w64, install pthreads-w64
 * Install libcurl devel (http://curl.haxx.se/download.html)
   * Make sure you have libcurl.m4 in MinGW\share\aclocal
   * Make sure you have curl-config in MinGW\bin
 * Install openssl devel (https://www.openssl.org/related/binaries.html)
 * In the MSYS shell, run:
   * ./autogen.sh	# only needed if building from git repo
   * LIBCURL="-lcurldll" ./configure CFLAGS="*-march=native*"
     * # Use -march=native if building for a single machine
   * make

#### Architecture-specific notes:
 * The miner currently does not support any architecture-specific optimizations.

Usage instructions
==================
Run "minerd --help" to see options.

### Connecting through a proxy

Use the --proxy option.

To use a SOCKS proxy, add a socks4:// or socks5:// prefix to the proxy host  
Protocols socks4a and socks5h, allowing remote name resolving, are also available since libcurl 7.18.0.

If no protocol is specified, the proxy is assumed to be a HTTP proxy.  
When the --proxy option is not used, the program honors the http_proxy and all_proxy environment variables.

Donations
=========
Donations for the work done in this fork are accepted at
* XPB: `PByFqCfuDRUPVvirsNrzrUXnuUdF7LpXsTTZXeq5cdHpJDogbJ8EBXopciN7DmQiGhLEo5ArA7dFqGga2AAhbRaZ2gL8jjp9VmYgk`
* BTC: `18Y2XaRRQZDZohkkBG1yXrWXMD1DBNtyA8`

Credits
=======
Pebbleminer was forked from [lucasjones](https://github.com/lucasjones)'s [CPUMiner-multi](https://github.com/lucasjones/cpuminer-multi), and has been developed by the Pebblecoin Developers.

License
=======
GPLv2.  See COPYING for details.
