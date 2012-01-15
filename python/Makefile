include ../Makefile.defs

INCLUDES += -I.. -I/usr/include/python2.7

lib:
	swig -c++ -python -o ce_via_td_wrap.cc ../ce_via_td.i
	$(CC) $(CFLAG) -fPIC -c ../ce_via_td.cc -o ce_via_td.o
	$(CC) $(CFLAG) -fPIC -c ce_via_td_wrap.cc $(INCLUDES)
	$(CC) -g -shared -Wl -o _CEviaTD.so ce_via_td.o ce_via_td_wrap.o -lstdc++

clean:
	rm -f *~ *.o *.so *_wrap.cc CEviaTD.py *.pyc
