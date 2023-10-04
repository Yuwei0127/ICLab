LIB:
	g++ -c testlib.cpp -o testlib.o -I./
	g++ -o testlib testlib.o -L. ICSCLib.a -ldl -lnsl -lsocket

DLL:
	g++ -c testdll.cpp -o testdll.o -I./
	g++ -o testdll testdll.o -L./ ICSCDLL.so libssl.so.0.9.7 libcrypto.so.0.9.7

clean:
	rm *.o
	rm testdll
	rm testlib

