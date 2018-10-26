#
#	CHEN SEI-LIM
#	2017-03-07
#

TOPDIR= $(PWD)


AR=			ar
CC=  		/usr/vacpp/bin/xlc
CXX=		/usr/vacpp/bin/xlC
LD=			ld
CFLAGS=		-g -q32 -O3 -lpthread -D__AIX__ -I$(TOPDIR)/src -I$(TOPDIR)/oem/unix
CXXFLAGS=	$(CFLAGS) \
			-I$(TOPDIR)/src -I$(TOPDIR)/oem/unix
LDFLAGS=	-lpthread -lbsd -lrt -lc

OBJS_GW= \
		src/accsox.o \
		src/global.o \
		src/accsox_sock.o\
		src/accsox_proc.o\
		src/accsox_file.o\
		src/main.o


.SUFFIXES:.cpp .o

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@



all:	accsox



accsox:	$(OBJS_GW)
	$(CC) -o $(TOPDIR)/bin/$@ $(OBJS_GW) $(LDFLAGS)


install:	accsox
	sh $(TOPDIR)/tool/install.sh


clean:
	rm -rf $(TOPDIR)/bin/* $(OBJS_GW)

