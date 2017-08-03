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

void error_cd(struct evconnlistener *lev, void *arg)
{
	int err = EVUTIL_SOCKET_ERROR();
	fprintf(stderr, "error[%d]: %s \n", err,
			evutil_socket_error_to_string(err));
}	

void event_cb(struct bufferevent *bev, short events, void *ctx)
{
	if (events & BEV_EVENT_ERROR)
        perror("Error from bufferevent");
    	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        	bufferevent_free(bev);
    	}
}

void write_cb(struct bufferevent *bev, void *ctx)
{

}

void read_cb(struct bufferevent *bev, void *ctx)
{
   	struct evbuffer *input = bufferevent_get_input(bev);
    	struct evbuffer *output = bufferevent_get_output(bev);
    	/* Copy all the data from the input buffer to the output buffer. */
	printf("read [%d]\n", evbuffer_get_length(input));
    	evbuffer_add_buffer(output, input);	
	
}

void accept_cb(struct evconnlistener* lev, evutil_socket_t fd,
	       struct sockaddr* addr, int len, void *ptr)
{
	fprintf(stdout, "accept [%s]\n", inet_ntoa(((struct sockaddr_in*)addr)->sin_addr));
	struct event_base *base = evconnlistener_get_base(lev);
	if(NULL == base)
		return;

	struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, read_cb, write_cb, event_cb, &fd);
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
	sin.sin_port = htons(2333);

	struct evconnlistener *lev = evconnlistener_new_bind(
			base, accept_cb, NULL, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, 
			-1,(struct sockaddr*)&sin, sizeof(sin));
	if(NULL == lev){
		fprintf(stderr, "evconnlistener_new_bind faiiled!");
		event_base_free(base);
		return -1;
	}

	evconnlistener_set_error_cb(lev, error_cd);
	event_base_dispatch(base);
	return 0;
}
