include ../Makefile.defs

INCLUDES += -I.. -I/usr/lib/perl/5.12.4/CORE

lib:
	swig -c++ -perl -shadow -o ce_via_td_wrap.cc ../ce_via_td.i
	$(CC) $(CFLAG) -fPIC -c ../ce_via_td.cc -o ce_via_td.o
	$(CC) $(CFLAG) -fPIC -c ce_via_td_wrap.cc $(INCLUDES)
	$(CC) -g -shared -Wl -o CEviaTD.so ce_via_td.o ce_via_td_wrap.o -lstdc++

clean:
	rm -f *~ *.o *.so *_wrap.cc *.pm
