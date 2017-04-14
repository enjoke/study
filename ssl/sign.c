#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/md5.h>
#include <openssl/pem.h>
#include <openssl/objects.h>
#include <openssl/hmac.h>
#include <openssl/pkcs12.h>
#include <openssl/ssl.h>
#include "apps.h"

int gen_sign(char *md, int md_len, char *sign, char *err)
{
	int ret = -1, sign_len;
	char file[128];
	char sign_data[2048];
	EVP_MD_CTX              *md_ctx = EVP_MD_CTX_new();

	sprintf(file, "%s/der/alipay_pri.pem", getenv("HOME"));
	EVP_PKEY *pkey = NULL;
	pkey = load_key( file, FORMAT_PEM, 0, "12345", NULL, "Private key" );
	if(!pkey){
		sprintf(err, "load key [%s] failed!\n", file);
		goto _SIGN_END;
	}

	EVP_SignInit( md_ctx, EVP_sha256() );
	EVP_SignUpdate( md_ctx, md, strlen(md) );
	ret = EVP_SignFinal( md_ctx, (unsigned char*)sign_data, (unsigned int*)&sign_len, pkey );
	if(1 != ret){
		ERR_print_errors_fp( stderr );
                goto _SIGN_END;
	}

	ret = EVP_EncodeBlock((unsigned char *)sign, (unsigned char*)sign_data, sign_len);
_SIGN_END:
	if( md_ctx )    EVP_MD_CTX_free(md_ctx);
	if(pkey)        EVP_PKEY_free( pkey );                                                                                    
	return ret;
}

int verify_sign(char *sign, int sign_len, char *md, int md_len, char *err)
{
	int ret = -1, rsa_len;
	char file[128];
	EVP_MD_CTX    *md_ctx = EVP_MD_CTX_new();
	char b64[2048];
        EVP_PKEY *pkey = NULL;

	sprintf(file, "%s/der/alipay_pub.pem", getenv("HOME"));
        pkey = load_pubkey( file, FORMAT_PEM, 0, NULL, NULL, "public key" );
        if(!pkey){
                sprintf(err, "load key [%s] failed!\n", file);
                goto _VERIFY_END;
        }

	ret = EVP_DecodeBlock((unsigned char*)b64, (unsigned char*)sign, sign_len);
	if(ret <= 0){
		sprintf(err, "b64½âÂëÊ§°Ü!\n");
		goto _VERIFY_END;
	}
	
	for(rsa_len=ret; b64[--ret]=='\0';rsa_len--);
	
	EVP_VerifyInit( md_ctx, EVP_sha256() );                                                                  
        EVP_VerifyUpdate( md_ctx, md, md_len );                                                                   
                                                                                                                  
        ret = EVP_VerifyFinal( md_ctx, (unsigned char*)b64, rsa_len, pkey );
	if(1 != ret)	
		sprintf(err, "verify failed%d!\n", ret);
	
_VERIFY_END:
        if( md_ctx )    EVP_MD_CTX_free(md_ctx);                                                                  
        if(pkey)        EVP_PKEY_free( pkey );                                                                                    
        return ret; 
}

int main(int argc, char **argv)
{
	char md[]  = "Hello world!";
	char sign[2048], err[512];
	int  ret;
	ERR_load_crypto_strings();

	ret = gen_sign(md, strlen(md), sign, err);
	if(ret <= 0){
		printf("Ç©ÃûÊ§°Ü %s\n", err);
		return -1;
	}
	ret = verify_sign(sign, ret, md, strlen(md), err);
	if(1 != ret)	printf("ÑéÇ©Ê§°Ü %s\n", err); 
	else		printf("ÑéÇ©³É¹¦!\n");
        CRYPTO_cleanup_all_ex_data();
	return ret;
}
