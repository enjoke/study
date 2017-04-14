#include <stdio.h>
#include <stdlib.h>
        
#include<string.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/md5.h>
#include <openssl/pem.h>
#include <openssl/objects.h>
#include <openssl/hmac.h>
        
#define PUB "/home/new/der/alipay.pem"

int main(int argc, char **argv)
{
	EVP_PKEY *pkey = NULL;
	FILE	 *fp   = NULL;

	fp = fopen(PUB, "r");
	if(NULL == fp){
		printf("open pem fail!\n");
		return -1;
	}

	pkey = PEM_read_PUBKEY(fp, NULL, NULL, NULL);
	if(NULL == pkey){
		printf("read pem fail!\n");
		return -1;
	}
	
	RSA_print_fp(stdout, pkey->pkey.rsa, 0); 
	printf("%d\n", RSA_size(pkey->pkey.rsa));

	return 0;	
}
