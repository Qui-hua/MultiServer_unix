#include <accsox.h>
#include <config.h> 
#include <global.h> 





/*
 *	int
 *	accsox_proc (ACCSOX *accsox)  
 */
int
accsox_proc (ACCSOX *accsox)
{
	int		sock = accsox->sock;
	
	int		left = 0;
	int		num = 0;
	int		i = 0;
	char 	OPCODE[4];
	char    buf[20];
	
	logger(INFO,"%04d\t accsox_proc: -\n", accsox->num);
	left = recv_data( sock, (char *)accsox->data, accsox->time_out,(char *)accsox->fromIP,MSG_SIZE);

	if(left <= 0)
	{
		logger(ERROR,"%04d\t accsox_proc: recv error: %s-\n",accsox->num,strerror(errno));
		goto fail;
	}
	
	logger(INFO,"%04d\t accsox_proc: recv_data-\n", accsox->num);
	send_data (sock, (char *)accsox->data, MSG_SIZE );
	
	logger(INFO,"%04d\t accsox_proc: send_data-\n", accsox->num);
		
	return -1;
//	return left;
	
	
	fail:
		return -1;
}


