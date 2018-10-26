#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <fcntl.h>

#include <errno.h>
int c=0;
///��]��///

int rcv_INPLSL_INPRCN_data(int sock_client, int INPLSL, int INPRCN1,
		int INPRCN2, int Timeout)   ///////收INPLSL INPRCN data
		{

	int lodlength1 = 0, lodlength2 = 0, R_size = 0, rcv_size = 0,
		total_rcv1 = 0, total_rcv2 = 0, rv = 0 , success = 0;

	int will_recv_le1 = 0, will_recv_le2 = 0, buf_int1 = 0, buf_int2 = 0;
	char buf[50],path[150];
	
	will_recv_le1 = INPRCN1 * INPLSL;
	for (int i = 1; i <= INPLSL; i = i * 10) {
		buf_int1 = ((INPLSL % (i * 10)) - (INPLSL % (i))) / i; //取得 要計算的INPLSL位
		buf_int1 = buf_int1 * INPRCN2;
		if (buf_int1 >= (100000000 / i)) {
			will_recv_le1 = will_recv_le1 + buf_int1 / (100000000 / i);
		}
		will_recv_le2 += buf_int1 % (100000000 / i) * i;

		if (will_recv_le2 >= 100000000) {
			will_recv_le1 += will_recv_le2 / (100000000);
			will_recv_le2 = will_recv_le2 % (100000000);
		}
	}

	char buff[10001], out[200];

	memset(buff, 0, sizeof(buff));

	struct timeval m_stTimeout;

	m_stTimeout.tv_sec = (time_t) Timeout;
	m_stTimeout.tv_usec = 0;
	fd_set fdRead;
	FD_ZERO(&fdRead);
	FD_SET(sock_client, &fdRead);

	while (lodlength1 <= will_recv_le1) {
		while (lodlength2 < will_recv_le2) {
			int iRet = select(sock_client + 1, &fdRead, NULL, NULL,
					&m_stTimeout);
			if (iRet <= 0) {
				printf("[%d]rcv_INPLSL_INPRCN_data %ds not recv, error!!!\n",
						sock_client, Timeout);
				sprintf(out, "[%d]rcv_INPLSL_INPRCN_data recv %d%d bytes\n",
						sock_client, total_rcv1, total_rcv2);
				return -1;
			}
			rcv_size = (will_recv_le2 - lodlength2) % 10000;

			if (rcv_size > 10000 || rcv_size == 0)
				rcv_size = 10000;
			R_size = read(sock_client, buff + lodlength2, rcv_size);
			

			if (R_size == 0) {
				printf("[%d]rcv_INPLSL_INPRCN_data recv size =%d,err!!!\n",
						sock_client, R_size);
				return 0;
			} else if (R_size < 0) {
				sprintf(buf, "[%d]read error,%s!!!", sock_client,
						strerror(errno));
						
				return -2;
			}
			lodlength2 = lodlength2 + R_size;
			total_rcv2 += R_size;
			if (lodlength2 == 10000) {
				lodlength2 = lodlength2 - 10000;
				will_recv_le2 = will_recv_le2 - 10000;
				memset(buff, 0, sizeof(buff));
			}

		}
		if (total_rcv2 >= 100000000) {
			total_rcv2 = total_rcv2 - 100000000;
			total_rcv1++;
		}

		lodlength1++;
		lodlength2 = 0;
		will_recv_le2 = 100000000;
	}
	printf("[%d]rcv_INPLSL_INPRCN_data Receive %d%d bytes\n", sock_client,
			total_rcv1, total_rcv2);
	printf("[%d]\n", strlen(buff));

//	printf("%s-\n", buff);
	return 1;

}

int rcv_n_data(int sock_client, int INPHDL, int Timeout)   ///////收N data
		{
	int lodlength = 0, R_size = 0, rcv_size = 0, total_rcv = 0, rv = 0 , success=0;;
	char buff[10001], buf[30],path[120];


	struct timeval m_stTimeout;
	m_stTimeout.tv_sec = (time_t) Timeout;
	m_stTimeout.tv_usec = 0;
	fd_set fdRead;
	FD_ZERO(&fdRead);
	FD_SET(sock_client, &fdRead);

	while (lodlength < INPHDL) {
		rcv_size = (INPHDL - lodlength) % 10000;
		if (rcv_size > 10000 || rcv_size == 0)
			rcv_size = 10000;
		int iRet = select(sock_client + 1, &fdRead, NULL, NULL, &m_stTimeout);
		if (iRet <= 0) {
			printf("[%d]rcv_n_data %ds not recv, error!!!\n", sock_client,
					Timeout);
			return -1;
		}
		R_size = read(sock_client, buff + lodlength, rcv_size);
		if (R_size == 0) {
			printf("[%d]rcv_n_data recv size =%d,error!!!\n", sock_client,
					R_size);
			return 0;
		} else if (R_size < 0) {
			sprintf(buf, "[%d]read error,%s!!!", sock_client, strerror(errno));
			return -2;
		}
		lodlength = lodlength + R_size;
		total_rcv += R_size;
		if (lodlength == 10000) {
		///	printf("rv[%s]\n",buff);
			/*
			rv=write_data((char*) buff, (char*) path);
			printf("rv[%d]\n",rv);
			if(rv==-1)
				success=-1;
			*/
			lodlength = lodlength - 10000;
			INPHDL = INPHDL - 10000;
			memset(buff, 0x0, sizeof(buff));
		}
		//cout<<total_rcv<<"\n";
	}
	printf("[%d]rcv_n_data Receive %d bytes\n", sock_client, total_rcv);
	printf("%s-\n", buff);
	
	return total_rcv;
}


void *show_message( void *ptr )
{
          struct sockaddr_in server;  
		  int sock,R_Wsize;  
		  char buf[2000],buf1[2000];  
		  int n,INPLSL,INPRCN,stt;
		  
		  /* �s�@ socket */  
		  sock = socket(AF_INET, SOCK_STREAM, 0);  
		  printf("sock%d\n", sock);
		  /* �ǳƳs�u�ݫ��w�Ϊ� struct ��� */  
		  server.sin_family = AF_INET;  
		  char *message;
          message = (char *) ptr;
		  server.sin_port = htons( atoi( message));
		  
		  /* 127.0.0.1 �O localhost ������} */  
		  inet_pton(AF_INET, "128.110.3.19", &server.sin_addr.s_addr);
		  
		  /* �P server �ݳs�u */  
		  stt= connect(sock, (struct sockaddr *)&server, sizeof(server));
		  printf("connect%d\n", stt);
		  printf("connect  error!!!%s",strerror(errno));
		  /* �q���A��������� */  
		  for(int i=0;i<3;i++)
	  {
		  memset(buf, 0, sizeof(buf));  
		  memset(buf, 65, 1500);  
		  
		  //usleep(1000000);/*1S*/
		  //char a;
		 // scanf("%c", &a);
		 // buf[0]=a;
		time_t t = time(NULL);
	    struct tm tm = *localtime(&t);
	    struct  timeval    tv;
	    gettimeofday(&tv,NULL);
	    int year=tm.tm_year + 1900,mon=tm.tm_mon + 1,d=tm.tm_mday,h=tm.tm_hour,m=tm.tm_min,s=tm.tm_sec,ms=tv.tv_usec/10000;
	    char times[20];
	    sprintf( times, "%04d%02d%02d%02d%02d%02d%02d\t",year,mon,d,h,m,s,ms);
	    memcpy (buf+70,times,16);
	    int a;
	    char b[10];
	    sprintf( b, "%04d",c);
	    memcpy (buf+86,b,4);
	    a=344;
	    sprintf( b, "%05d",a);
	    memcpy (buf+300,b,5);
	    INPLSL=0;
	    sprintf( b, "%05d",INPLSL);
	    memcpy (buf+305,b,5);
	    INPRCN=0;
	    sprintf( b, "%010d",INPRCN);
	    //sprintf( b, "9999999999");
	    memcpy (buf+310,b,10);
	  //  	usleep(10000);////1S
	  
	  	buf[6]='0';
	  	if(i==0)
	  	{
	  		sprintf( b, "C01S");
	  		memcpy (buf,b,4);
		}
	  	else if(i==1)
	  	{
	  		sprintf( b, "C01T");
	  		memcpy (buf,b,4);
		}
	  	else if(i==2)
	  	{
	  		sprintf( b, "E01E");
	  		memcpy (buf,b,4);
		}
		printf("+++++++++++++++%s+++++++++++++++++++\n",b);
	  	a=5;
	    sprintf( b, "%05d",a);
	    memcpy (buf+120,b,5);
	    memcpy (buf+125,b,5);
	  	n=write(sock, buf, 500);
	  	n=write(sock, buf+500, 500);
	 	n=write(sock, buf+1000, 500);
	 	
		printf("\t[Info] send %d bytes: %c\n", n, buf[0]);  
	 	if(i==1)
	 	{ 
	 		memset(buf, 67, sizeof(buf));  
	 			
	 		//for(int j=0;j<30;j++)
	 		//	n=write(sock, buf, 1000);
	 			n=write(sock, buf, 344);  //N
	 		//usleep(1000000000);////1S
			memset(buf, 68, sizeof(buf)); 
	 		for(int j=0;j<INPLSL;j++)
	 			{
	 				int send_maxL=2000,had_s=0,send_L=0;
					while(had_s<INPRCN)
					{
						send_L=INPRCN-had_s;
						if(send_L>send_maxL)
							send_L=send_maxL;
						n=write(sock, buf, send_L);
						had_s+=n;
					}
				
	 				
					printf("\t[Info] INPLSL send %d bytes\n", had_s);  
				}
		}
		memset(buf, 0, sizeof(buf));  
		//	usleep(100000000);////1S
	    //usleep(100000000);
		n = read(sock, buf, sizeof(buf));
		printf("============rcv data=====================\n%s\n", buf);  	  
		printf("\t[Info] Receive %d bytes\n", n);  
		
		if(i==1)
		{
			int INPHDL,INPLSL,INPRCN1,INPRCN2;
			memset(buf1, 0, sizeof(buf1));
			memcpy(buf1, buf + 300, 5);
			INPHDL = atoi(buf1);
			memset(buf1, 0, sizeof(buf1));
			memcpy(buf1, buf + 305, 5);
			INPLSL = atoi(buf1);
			memset(buf1, 0, sizeof(buf1));
			memcpy(buf1, buf + 310, 2);
			INPRCN1 = atoi(buf1);
			memset(buf1, 0, sizeof(buf1));
			memcpy(buf1, buf + 312, 8);
			INPRCN2 = atoi(buf1);
			if (INPHDL != 0)
				R_Wsize = rcv_n_data(sock, INPHDL, 5);
			
			if(INPLSL!=0 &&(INPRCN1!=0 ||INPRCN2!=0))
				R_Wsize = rcv_INPLSL_INPRCN_data(sock, INPLSL, INPRCN1,
					INPRCN2, 5);
		}

	  	
	  }
	  	
		  
		  /* ���� socket */  
		close(sock);  
		return 0;  
	  
}


int main(int argc, char *argv[]){
   pthread_t thread1;
   char * message1 = "5429";
   c=atoi(*++argv);
		  // printf ("%sn", *++argv);
   void *ret;
   pthread_create(&thread1, NULL , show_message , (void*) message1);
   pthread_join( thread1, &ret);
   printf("client over\n");

   return 0;
}


