LIB:
	g++ -c testlib.cpp -o testlib.o -I./
	g++ -o testlib testlib.o -L. ICSCLib.a -ldl

DLL:
	g++ -c testdll.cpp -o testdll.o
	g++ -o testdll testdll.o -L. ICSCDLL.so ICSssl.so ICScrypto.so

clean:
	rm *.o
	rm testdll
	rm testlib

