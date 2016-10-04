CC  = gcc
CCC = g++
LD  = ld
AR  = ar
BIN = testHiSecure.bin
BIN2 = DeidDekey
CCCFLAGS = -g -Wno-write-strings -Wno-unused-result $(INCS)
OBJS=TestHiSecure.o TestBasic.o TestCert.o TestCRL.o TestOCSP.o TestAsym.o PKCS8SoftKey.o HashAndSymEncryption.o
OBJS2=deid_dekey.o TestBasic.o TestCert.o TestCRL.o TestOCSP.o TestAsym.o PKCS8SoftKey.o HashAndSymEncryption.o
LIBS = -L../lib/Linux -L./ -lHiSecure -lpkcs11lib -lcrypto -ldl -lHicos_p11v1
INCS = -I. -I../include -I../openssl_linux

all: clean $(BIN) $(BIN2)

clean:
	rm -f *.o $(SO) $(STATIC)

#libHiSecure:$(OBJS) $(SO)

DeidDekey: $(OBJS2)
	$(CCC) -g -o $(BIN2) $(OBJS2) $(LIBS)
testHiSecure.bin: $(OBJS)
	$(CCC) -g -o $(BIN) $(OBJS) $(LIBS)
deid_dekey.o:
	$(CCC) $(CCCFLAGS) -c deid_dekey.cpp
TestHiSecure.o:	
	$(CCC) $(CCCFLAGS) -c TestHiSecure.cpp
TestBasic.o:	
	$(CCC) $(CCCFLAGS) -c TestBasic.cpp
TestCert.o:	
	$(CCC) $(CCCFLAGS) -c TestCert.cpp
TestCRL.o:	
	$(CCC) $(CCCFLAGS) -c TestCRL.cpp
TestOCSP.o:	
	$(CCC) $(CCCFLAGS) -c TestOCSP.cpp
TestAsym.o:	
	$(CCC) $(CCCFLAGS) -c TestAsym.cpp
PKCS8SoftKey.o:
	$(CCC) $(CCCFLAGS) -c PKCS8SoftKey.cpp
HashAndSymEncryption.o:
	$(CCC) $(CCCFLAGS) -c HashAndSymEncryption.cpp
