#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <zlib.h>
#include <zconf.h>

#define CHUNK 16384

/* ps: gcc -o x x.c -crytpo -lz*/
/* zip字符解压缩 */
int decompress(unsigned char *src, unsigned int src_len, FILE *fp)
{
	int ret = -1, cur_len, i, act;
	unsigned char in[CHUNK], out[CHUNK];
	z_stream strm;

	strm.zalloc = Z_NULL;  
	strm.zfree = Z_NULL;  
	strm.opaque = Z_NULL;  
	inflateInit(&strm);
	
	cur_len = src_len;
	
	for(i=0; Z_STREAM_END != ret && 0 != cur_len; i++){
		if(cur_len < CHUNK) 	strm.avail_in = cur_len; 
		else{
			strm.avail_in = CHUNK;
			cur_len -= CHUNK;
		}
		
		memcpy(in, src+i*CHUNK, strm.avail_in);
		strm.next_in = in;
	
		do{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			act = CHUNK - strm.avail_out;
		   	fwrite(out, 1, act, fp);
		}while(0 == strm.avail_out);
	}

	inflateEnd(&strm);
	
	if(Z_STREAM_END == ret) ret = Z_OK;

	return ret;
}

int main(int agrc, char **agrv)
{
	char buf[] = "eJwL8GZmEWFgYOBgcIwq92zZdv5hCxMDQ5gJA4MsUNTTz8/Q3MDQ0NLCIso33sLSwtDQ0MTcyNLCwNDI9O2s3X3BhgJt1Sr3HppkalyIqdFlvKi05Alrk4iKyrbGpuvpczZuFp/mePz2w8LJbzr/18+d4//4/jfLY1tamVoYNnhOnh6Q/fR19amQz2weDQJKy3UF1bzy3ZgPXBJZfFGNaeck88Al1XOlTrLfqHr88vFHFv5oj0Rp3182l+W+3bOz+TX97Mfz/26Eftlzf980+V6x+T3L/xf9z7hm7jDBmqHU+sj3/XrxdvPr59yv+/vt76eXNeWWH3bPZ9cq+lmys/rRp+/71zXX/fxeV1n991UGg9jGFj4JkWl2vxmD50pJNhwtkp7EvI0vwZJtUj/jRiGlwHvcNy6snMgYUXBgGssah2dxB7MY+IRF36wKVs7uWnJ7gsyGUobVR75GlT/RS0yYxeBTmpXCMO1yrgffDauGQJ2VIWsTdvqWQdRrN1eyRgtuOBQaVHU+4cyWalHLEqBit31dMYIGJ7a6yTWcTDFnnfHiygmmM9o3uhoSNXLbVzU2NWq9v7dMwsp0U9jE79tPMEazG6w45P2qgTNSWBRs2bF9vhuVEmI0l9xwrjuTmKQeDxRsrYl3XccRcSzmFMdPhcQstWbTuVKRwUuYtwEVLtj7euqKs8nGoj5A20MNwAr1In6WhZiIfgpd7iaWIPcpnIUHyG+59OjdBHugIlnRnyI7lRIkk3bbVy3zmDQl7ZzSbL9enQN7mtZySAgG2Gs5PTlRuKwJZEf6xHkHw1jWtH424NvN+ejI5wSgt7kN7p+MF9zwTcL3gNynzo3nWv4sC1ZmNvjSe02UUZsn8FIg/2tVP9t1l8HOOZ/gzaQmzt3DNa3gux0o4EtvvG1YyzljwUQ31u5ELbBHVnvMV/offqCOOQCefLsnm3jUHPTQDGQE8jgZGOSAokEuRgaGBpAUbGCIkX5DbnsddhCojb9R/0J8yl3VrEnzP74+kjQ/6XLLhGtuzDvDZi3LVQpyXdHaJPVqtppGmaZTUq6Jgqf8nO03LecYp8+4GfW2X+nDlBv1lvYbZz2rjt78+1Vh+FfnUjG/1uUmfEwcGaE9+nIV05INj51hfBjRcvJe/tYzO62F/yg80A2vF6vhmOP526NXtDU9wsXAaUXC5ykXZklct3GskE88s7b6yuRpB9nDny1ed3LSl1XZ13rXfX2unF+w2uTu5hgv67ybml5NKwo5XgRssX5vxLs0v+nxntrEBefvLhXJnTqhXdn2fviLkulXbmequAo8Sb58qjtHT+3oK+kGU46mw7KcHIcKBfiFKnfcVFd8yXN18+mWVzyT4/9rps+YO+vwluSccwvWWDpcyK54fOCk1v/7zjc3/pouJrv5+o9zNZLVCmq3r2Sz3ux6vlr6Rc5Vfce7O8onv9oTvrdyfuBX/y+MAd6MTCIMuEsTEGBkUICy8JYtyEZhi1kIABm1n4lQPAd4s7KBVDMB4XQg7cMC4gEAY9UH3w==";

	char hex[4096];
	char zbuf[10240];	
	/* openssl b64解码 */
	int len = EVP_DecodeBlock((unsigned char*)hex, (unsigned char *)buf, strlen(buf));	
	if(len <=0 ){
		printf("b64解码失败!\n");
		return -1;
	}

	int hex_len = len;
	for(;'\0'==hex[--len]; hex_len--);
	
	FILE *fp = fopen("xxx.zip", "wb");
	if(!fp){
		printf("打开文件失败!\n");
		return -1;
	}
	
	int ret = decompress((unsigned char*)hex, hex_len, fp);
	if(Z_OK != ret)
		printf("解压失败! %d\n", ret);

	fclose(fp);
	return 0;
}
