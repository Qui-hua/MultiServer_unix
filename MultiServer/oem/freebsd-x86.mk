#
#	CHEN SEI-LIM
#	2005-06-26
#

OSTYPE	= unix
LIBDIR	= /usr/local/lib

MAKE=/usr/local/bin/make
CC	= cc
CXX	= c++
AR	= ar
LD	= ld
CP	= cp
RM	= rm -rf
STRIP	= strip

CFLAGS	= -O3 -Wall -pipe -DFREEBSD -Wmissing-prototypes
LDFLAGS	= -L/usr/local/lib -pthread

export MAKE
export CC
export CXX
export AR
export LD
export CP
export RM
export STRIP

