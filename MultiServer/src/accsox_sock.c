

#include <accsox.h>
#include <config.h> 
#include <global.h> 



/*
 *	int
 *	accsox_bind (char *host, int port)
 */
int
accsox_bind (char *host, int port)
{
	struct 	sockaddr_in skaddr;
	int		sock = 0;
	int		rv = 0;
	struct linger linger;

	logger(WRITE,"\t\t accsox_bind: IP:%s,Port:%d-\n",host,port);
	sock = socket (AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		logger (ERROR,"\t\t accsox_bind: socket error %s ",strerror(errno));
		return -1;
	}

	
	linger.l_onoff  = 1;
	linger.l_linger = 0;

	rv = setsockopt (sock, SOL_SOCKET, SO_LINGER, (char*) &linger, sizeof (struct linger));
	if (rv < 0) {
		logger (ERROR,"\t\t accsox_bind: setsockopt error %s",strerror(errno));
		return 0;
	}
	int nodelay = 1;
	rv = setsockopt (sock, IPPROTO_TCP, TCP_NODELAY, (char*) &nodelay, sizeof (nodelay));
	if (rv < 0) {
		logger (ERROR,"\t\t accsox_bind: setsockopt error %s",strerror(errno));
		return 0;
	}	
	skaddr.sin_family = AF_INET;
	skaddr.sin_port = htons(port);
	skaddr.sin_addr.s_addr = inet_addr ((const char*) *host);

	rv = bind (sock, (struct sockaddr*) &skaddr, sizeof (skaddr));
	if (rv < 0) {
		logger (ERROR,"\t\t accsox_bind: bind error %s",strerror(errno));
		goto fail;
	}

	rv = listen (sock, BACK_LOG);  //// 後面是連線上限
	if (rv < 0) {
		logger (ERROR,"\t\t accsox_bind: listen error %s",strerror(errno));
		return 0;
	}

	logger (INFO,"\t\t accsox_bind: [%s:%d] OK\n", host, port);
	
	return sock;

fail:

	close (sock);

	return -1;
}



/*
 *	int
 *	accsox_bind (int sock, char *data, int Timeout, char* fromIP, int length )
 */
int
recv_data (int sock, char *data, int Timeout, char* fromIP, int length )
{
	int num;
	struct	timeval tv;
	fd_set	r_copy;
	FD_ZERO (&r_copy);
	FD_SET (sock, &r_copy);

	tv.tv_sec = Timeout;
	tv.tv_usec = 0;
	int  R_size=0;
    int left=0;
	while(1)
	{
		num = select (sock + 1, &r_copy, NULL, NULL, &tv);
		if (num <= 0 && errno != EINTR)
		{
			logger (ERROR,"\t\t recv_data: select error,errno=%d,%s!!!\n",errno, strerror(errno));
			return -1;
		}
		R_size = read (sock, data+left, length-left);  // 先讀1500
		left = left + R_size;
		if(R_size == 0)
		{
			return 0;
		}
		else if(R_size < 0)
		{
			logger (ERROR,"\t\t recv_data: read error,%s!!!\n", strerror(errno));
			return -1;
		}
		if(left==length)
			break;
		
	}
	return left;
	
}


/*
 *	int
 *	send_data(int sock, char *data, int length )
 */
int
send_data (int sock, char *data, int length )
{
	
	int real=0,had_send=0;
	while(1)
	{
		real = write (sock, data + had_send, length - had_send);
		if(real<0)
		{
			logger (ERROR,"\t\t send_data: had_send=%d, length=%d,error %s-\n",had_send,length, strerror(errno));
			return had_send;
		}
		had_send=had_send+real;
		if(had_send==length)
			break;
	}
	logger (DEBUG,"\t\t send_data: had_send=%d, length=%d-\n",had_send,length);
	return had_send;
	
}



