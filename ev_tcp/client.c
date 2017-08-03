#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>  

int main()
{
	char snd[] = "Hi libevent!";
	char rcv[128];
	struct sockaddr_in sin;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port = htons(6666);

	int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);;
	if(connect(fd, (struct sockaddr*)&sin, sizeof(struct sockaddr)) == -1){
		fprintf(stderr, "connect failed!\n");
		return -1;
	}

	if(-1 == send(fd, snd, strlen(snd), 0)){
		fprintf(stderr, "send failed!\n");
		goto __END__;
	}

	if(-1 == recv(fd, rcv, 127, 0)){
		fprintf(stderr, "recv failed!\n");
		goto __END__;
	}
	printf("%s\n", rcv);
__END__:
	close(fd);
	return 0;
}
