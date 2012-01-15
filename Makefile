include Makefile.defs

INCLUDES += -I/usr/include/tidy
LIBS += -lm -ltidy

all:content_extraction

.PHONY:content_extraction perl python

content_extraction:content_extraction.cc ce_via_td.o html_tidy.o
	$(CC) -o $@ $^ $(CFLAG) $(INCLUDES) $(LIBS)

perl:
	$(MAKE) -C perl

python:
	$(MAKE) -C python

.cc.o:
	$(CC) -o $@ -c $^ $(CFLAG) $(INCLUDES) 

clean:
	rm -f *~ *.o *.so *_wrap*.cc
	rm -f content_extraction 
	$(MAKE) -C perl clean
	$(MAKE) -C python clean

