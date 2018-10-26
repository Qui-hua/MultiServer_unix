#
#	CHEN SEI-LIM
#	2006-06-28
#


.SUFFIX	= .c .o


.c.o:	*.h
	$(CC) $(CFLAGS) -c $< -o $@

.cpp.o:	*.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

.cxx.o:	*.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o:	%.rc
	$(WINDRES) -o $@ $<


all:	 Makefile header $(OBJS) $(TARGET) $(LIBRARY) $(DYNAMIC) $(EXTRA)


header:
ifdef HEADER
	$(CP) $(HEADER) $(TOPDIR)/bin
endif # HEADER


ifdef TARGET
    $(TARGET):
	rm -f $(TOPDIR)/bin/$@
	$(CC) $(LDFLAGS) -o $(TOPDIR)/bin/$@ $(OBJS)
endif # TARGET


ifdef LIBRARY
    $(LIBRARY):
	rm -f $(TOPDIR)/bin/$@
	$(AR) -r $(TOPDIR)/bin/$@ $(OBJS)
endif # LIBRARY


clean:
	rm -f $(OBJS)

