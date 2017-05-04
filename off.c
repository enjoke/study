#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRU		http_params	/*结构体名*/
#define OFFSET(X, Y)	(unsigned int)((char *)&((X *)0L)->Y - (char *)0L)

#define OFFSETOF(X) 	printf("offset of %20.20s\t%d\n", #X, OFFSET(STRU, X));\

/* 计算结构体成员偏移量 */
void caloff()
{

}
int main(int argc, char **argv)
{
	caloff();
	return 0;
}


