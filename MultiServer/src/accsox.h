
#ifndef	_REMITS_H_
#define	_REMITS_H_


#define MSG_SIZE	    100
#define WRITE			0
#define ERROR			1
#define INFO			2
#define DEBUG			3


typedef struct _ACCSOX 			ACCSOX;


struct _ACCSOX {
	int		sock;
	char 	fromIP[20];
	int 	run_state;  ///初始0,C01S 1,C01T 2
	char 	data[MSG_SIZE];
	int		time_out;
	int		num;
};

extern ACCSOX *				pool_accsox;


#ifdef __cplusplus   
extern "C" {
#endif	/* __cplusplus */


extern int			accsox_init ();
extern int			accsox_bind (char *host, int port);
extern int	   		recv_data (int sock, char *data, int Timeout, char* fromIP, int  length );
extern				send_data (int sock, char *data, int length );

#ifdef __cplusplus
}
#endif	/* __cplusplus */


#endif	/* _REMITS_H_ */

