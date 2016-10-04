#include <stdio.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <stdlib.h> 
#include <stdint.h> 
#include <errno.h> 
#include <unistd.h>

#include "TestHiSecure.h"
#include "HiSecureDefine.h"
#include "HiSecureBasic.h"
#include "HiSecureAsym.h"
#include "HiSecureCert.h"
#include "HiSecureError.h"

#define ERROR_ACTION(ret); if(ret){printf("Error Code=%lX\n",ret); goto fail;}

int main(int argc, char **argv){

	PKCS11 PkcsData;
	unsigned long i, ret;
	unsigned long DeviceError;
	char *CardPin;
	char CardPin2[MAX_PIN_LENGTH];
	Key* CardKey;
	unsigned char *CipherData;
	int CipherLength;
	unsigned char *PlainData;
	int DataLength;
	char *CipherFile, *PlainFile;
	int cfd, pfd;
	int buffer_size = 2048;
	int c_size=256;
	int p_size=256;
	unsigned long SlotID[10];
	unsigned long SlotIDNum = 10;
	CipherLength = c_size;
	DataLength = p_size;

	ret = InitLibrary(MODULE_LIBRARY, &PkcsData);
	ERROR_ACTION(ret);
	
	CipherFile = argv[1];
	PlainFile = argv[2];
	if (!argv[3]){
		//strncpy(CardPin, "12345678", 9);
		printf("Please enter pin:\n");
		GETPIN(CardPin2);
		CardPin = CardPin2;

	}else{
		int slen = strlen(argv[3]);
		printf("s length = %i\n",  slen);
		if (slen > MAX_PIN_LENGTH){
			slen = MAX_PIN_LENGTH;
		}
		//strncpy(CardPin, argv[3], slen);
		CardPin = argv[3];
	}
	//printf("pin %s\n", CardPin);

	if(!CipherFile){
		printf("parameter for CipherFile error %s\n", CipherFile);
		ret=-1;
		goto errexit;
	}

	if(!PlainFile){
		printf("parameter for PlainFile error %s\n", PlainFile);
		ret=-1;
		goto errexit;
	}


	CipherData = (unsigned char *)malloc(buffer_size);
	PlainData = (unsigned char *)malloc(buffer_size);

	cfd = open (CipherFile, O_RDONLY);
	if (cfd == -1){
		printf("error open CipherFile %s\n", CipherFile);
		ret=-1;
		goto errexit;
	}

	pfd = open (PlainFile, O_RDWR|O_CREAT);
	if (pfd == -1){
		printf("error open PlainFile %s\n", PlainFile);
		ret=-1;
		goto errexit;
	}

	ret = read(cfd, CipherData, c_size); 
	if (ret != c_size){
		printf("CipherFile read error %i\n", ret);
		goto errexit;
	}


	ret = GetSlotID(&PkcsData, SlotID, &SlotIDNum);
	ERROR_ACTION(ret);

	for(i = 0 ; i < SlotIDNum; i++){
		if(IsTokenPresent(&PkcsData,SlotID[i])){
			break;
		}
	}

	if(i==SlotIDNum){
		printf("No Token avaliable.");
		goto fail;
	}
	
	ret = LoginToken(&PkcsData, SlotID[i], CardPin, &DeviceError);
	printf("ret %i\n", ret);
	if(ret == BASIC_PIN_INCORRECT){
		printf("PIN½X¿ù»~");
		if(DeviceError==0x0063C2) printf("-³Ñ¾l¨â¦¸¾÷·|");
		else if(DeviceError==0x0063C1) printf("-³Ñ¾l¤@¦¸¾÷·|");
		printf("\n");
	}
	ERROR_ACTION(ret);

	ret = GetEnciphermentKey(&PkcsData, &CardKey);
	ERROR_ACTION(ret);

	ret = BasicAsymDecrypt(CKM_RSA_PKCS, CipherData, CipherLength, CardKey, PlainData, &DataLength);
	ERROR_ACTION(ret);

	ret = write(pfd, PlainData, (size_t)DataLength);
	if (ret != DataLength){
		printf("Plain write error %i\n", ret);
		goto fail;
	}
	

	FreeKey(CardKey);
			
	ret = LogoutToken(&PkcsData);
	ERROR_ACTION(ret);
	printf("Decryption OK\n");
fail:
	CloseLibrary(&PkcsData);
errexit:
	close(cfd);
	close(pfd);
	return ret;

    return 0;
}
