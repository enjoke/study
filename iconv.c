#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h> 

#define SIZE	1024
int main(int argc, char **argv )
{
	char buf[208];
	size_t out = SIZE *SIZE;
	char utf[SIZE*SIZE + 1];
	char gbk[SIZE*SIZE + 1];

	/* ������ԭָ����в�����iconv()��ı�ָ�� */
	char *p = buf;
	char *q = utf;
	iconv_t cd = iconv_open("UTF-8", "GBK");  
    	if(cd == (iconv_t)-1) {  
        	perror("��ȡ�ַ�ת��������ʧ�ܣ�\n");  
        	return -1;  
    	}
	strcpy( buf, "{ \"rc\": \"UA\", \"rc_detail\": \"�û��Ѵ���\" }");
	size_t in = strlen(buf);
	if(iconv(cd, &p, &in, &q, &out) == (size_t)-1) {  
        	perror("fvck\n");  
        	iconv_close(cd);  
        	return -1;  
    	}  
    	iconv_close(cd);  
	printf("utf[%s]\n", utf);

	iconv_t cdx = iconv_open("GBK", "UTF-8");
        if(cdx == (iconv_t)-1) {
                perror("��ȡ�ַ�ת��������ʧ�ܣ�\n");
                return -1;
        }
	
	p = utf;
	in = strlen(utf);
	q = gbk; 
	out = SIZE *SIZE;
	if(iconv(cdx, &p, &in, &q, &out) == (size_t)-1) {
                perror("fvck\n");
                iconv_close(cdx);
                return -1;
        }
        iconv_close(cdx);
        printf("utf[%s]\n", gbk);
    	return 0;  
}  

