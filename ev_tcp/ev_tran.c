#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/util.h>

#include <errno.h>
#include <arpa/inet.h>
#include <sys/time.h>

struct timeval start, end;
int 	num = 0;

void error_cd(struct evconnlistener *lev, void *arg)
{
	int err = EVUTIL_SOCKET_ERROR();
	fprintf(stderr, "error[%d]: %s \n", err,
			evutil_socket_error_to_string(err));
	
	exit(-1);
}	

void event_cb(struct bufferevent *bev, short events, void *ctx)
{
	if((events & BEV_EVENT_READING) ||
	(events & BEV_EVENT_WRITING) ||
	(events & BEV_EVENT_ERROR)){
		free(bev);
		return;
	}
	else if(events & BEV_EVENT_TIMEOUT)
		printf("timeout!\n");
	else if(events & BEV_EVENT_EOF)
		printf("end!\n");
	else if(events & BEV_EVENT_CONNECTED)
		printf("connect!\n");
}

void write_cb(struct bufferevent *bev, void *ctx)
{
	struct bufferevent *first = (struct bufferevent*)ctx;
   	struct evbuffer *input = bufferevent_get_input(bev);
    	struct evbuffer *output = bufferevent_get_output(first);
	evbuffer_add_buffer(output, input);
	
}

void read_cb(struct bufferevent *bev, void *ctx)
{
   	struct evbuffer *input = bufferevent_get_input(bev);
    	/* Copy all the data from the input buffer to the output buffer. */
	printf("read [%d]\n", evbuffer_get_length(input));

	struct sockaddr_in sin;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port = htons(2333);

	struct bufferevent *con_bev = bufferevent_socket_new((struct event_base*)ctx, -1, BEV_OPT_CLOSE_ON_FREE); 
	if(NULL == con_bev){
		fprintf(stderr, "can not new a bev!\n");
		return;
	}

	if(-1 == bufferevent_socket_connect(con_bev, (const struct sockaddr *)&sin, sizeof(sin))){
		printf("can not connect!\n");
		return;
	}
	
	bufferevent_setcb(con_bev, write_cb, NULL, NULL, bev);
	bufferevent_enable(con_bev, EV_READ|EV_WRITE);
   	struct evbuffer *output = bufferevent_get_output(con_bev);
	evbuffer_add_buffer(output, input);
}

void accept_cb(struct evconnlistener* lev, evutil_socket_t fd,
	       struct sockaddr* addr, int len, void *ptr)
{
	if(0 == gettimeofday(&end, NULL)){
		double used = end.tv_sec - start.tv_sec + (end.tv_usec, start.tv_usec) / 1000000.00;
		printf("%ld, %ld,used [%.2lf] ", end.tv_sec, start.tv_sec, used);
	}
	printf("accept[%d] ", ++num);
	fprintf(stdout, "ip [%s]\n", inet_ntoa(((struct sockaddr_in*)addr)->sin_addr));
	struct event_base *base = evconnlistener_get_base(lev);
	if(NULL == base)
		return;

	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, read_cb, NULL, NULL, base);
	bufferevent_enable(bev, EV_READ|EV_WRITE);
}	

int main(int agrc, char** argv)
{
	struct sockaddr_in sin;
	struct event_base *base = event_base_new();
	if(NULL == base){
		fprintf(stderr, "event_base_new failed!\n");
		return -1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htonl(0);
	sin.sin_port = htons(6666);

	struct evconnlistener *lev = evconnlistener_new_bind(
			base, accept_cb, base, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, 
			-1,(struct sockaddr*)&sin, sizeof(sin));
	if(NULL == lev){
		fprintf(stderr, "evconnlistener_new_bind faiiled!");
		event_base_free(base);
		return -1;
	}

	evconnlistener_set_error_cb(lev, error_cd);
	gettimeofday(&start, NULL);
	event_base_dispatch(base);
	return 0;
}
