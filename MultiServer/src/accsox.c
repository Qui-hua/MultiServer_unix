
#include <accsox.h>
#include <config.h> 
#include <global.h> 


static int			get_config ();
static void			__accsox_intr (int signo);
void *				worker_main (void *param);
static void	*		worker_loop (void *param);

char *
info_vendor[] = {
	"*********************\n",
	"SYSCOM  BANK start 1.0.3 @ 2018-03-21. All rights reserved.\n",
	"*********************\n",
	NULL
};

static int			serv_accsox = -1;

EXPORT int sock0;

EXPORT int Program_run;    //程式是否執行 0停 1執行   
EXPORT int client_num;     //目前有多少連線
EXPORT int Con_Sizq;       //連接數量
EXPORT int first_Tout;     //預設time out
EXPORT int write_log_s;    //log 等級 0全部不寫 1只寫錯誤 2寫錯誤 完成 3全寫 
EXPORT int desc_logger; 

EXPORT char Server_Port[10];
EXPORT char today_Date[8];
EXPORT char config_path[256];
EXPORT char rootfs[256];
EXPORT char data_server_IP[20];
EXPORT char data_Server_Port[10];
EXPORT char server_IP[20];
EXPORT char Log_File[256]; 
EXPORT char *text_logger = NULL;

EXPORT pthread_mutex_t	lock_logger;
EXPORT pthread_mutex_t	lock_accept;

/*
 *	int
 *	accsox_init ()
 */
int
accsox_init() 
{
	///signal處理////
	signal (SIGHUP,  &__accsox_intr);
	signal (SIGINT,  &__accsox_intr);
	signal (SIGQUIT, &__accsox_intr);
	signal (SIGPIPE, &__accsox_intr);
	signal (SIGALRM, &__accsox_intr);
	signal (SIGTERM, &__accsox_intr);
	signal (SIGSEGV, &__accsox_intr);
	signal (SIGBUS,  &__accsox_intr);
	
	// 初始線程鎖
	pthread_mutex_init (&lock_logger, NULL);
	pthread_mutex_init (&lock_accept, NULL);
	

	////設定log 長度與log檔案//
	text_logger = (char*) malloc (sizeof (char) * MAX_LINE_LEN);
	memset (text_logger, 0x0, sizeof (char) * MAX_LINE_LEN);
	desc_logger = open (Log_File, O_RDWR | O_CREAT | O_APPEND, 0644);
	
	for (int i = 0; info_vendor[i]; i++) {
			logger (WRITE,info_vendor[i]);
		}
		
	/////給予初始值///////	

    client_num = 0   				//目前有多少連線
  ,	Con_Sizq = 0     				//連接數量
  , first_Tout = 0  				//預設time out
  , write_log_s = 3  				//log 等級 0全部不寫 1只寫錯誤 2寫錯誤 完成 3全寫 
  , Program_run = 1 				//程式是否執行 0停 1執行
  ;
	if(get_config() < 0)    
	{
		logger (ERROR,"\t\t accsox_init: get_config error, %s-\n",strerror(errno));
		goto fail;
	}
	
	
	// ACCSOX 資料群
	pool_accsox = (ACCSOX*) malloc (sizeof (ACCSOX) * Con_Sizq);
	memset (pool_accsox, 0x0, sizeof (ACCSOX) * Con_Sizq);
	logger (WRITE,"\t\t accsox_init: pool_accsox=0x%08x, Con_Sizq=%d-\n", (int) pool_accsox, Con_Sizq);

	
	///bind socket////
	
	
	
	
	serv_accsox = accsox_bind (server_IP,  atoi (Server_Port));
	if (serv_accsox < 0) {
		logger (ERROR,"\t\t accsox_init: bind error %s-\n",strerror(errno));
		goto fail;
	}
	
	
	return 0;
	fail:
		logger (ERROR,"\t\t accsox_init error-\n");
		exit (1);
		return -1;
}




/*
 *	int
 *	accsox_main (void)
 */
int
accsox_main (void)
{
	pthread_t pool_thread[Con_Sizq];
	pthread_t cobol_read;
	pthread_attr_t attr;
	pthread_attr_init (&attr);
	pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);
	int xrow = 0, st = 0;
	void *ret;
	
	// 建立線程
	for (xrow = 0; xrow < Con_Sizq; xrow++) {
			ACCSOX*	accsox = &pool_accsox[xrow];
			accsox->num = xrow;
			pthread_create (&pool_thread[xrow], &attr, worker_main, (void*)accsox );
	}
	logger (INFO,"accsox_main: All Pthread create OK \n");
	//Con_Sizq
	for (xrow = 0; xrow < Con_Sizq; xrow++) {
			st=pthread_join(pool_thread[xrow], &ret);
	}
}



/*
 *	void
 *	worker_main (void)
 */
void *
worker_main (void *param)
{

	struct	sockaddr_in skaddr;
	int 	sock;
	int		serv = serv_accsox;
	int 	remit_run  = 0;
	
	socklen_t skalen = sizeof (struct sockaddr);
	ACCSOX*	accsox = (ACCSOX*) param;
		// 初始線程鎖
	
	logger (INFO,"%04d\t worker_main: Pthread create \n",accsox->num);
	////持續等待client 連接///
	while(1){	
		pthread_mutex_lock (&lock_accept);
		logger (DEBUG,"%04d\t worker_main: Pthread wait accept\n",accsox->num);
		sock = accept (serv, (struct sockaddr*) &skaddr, &skalen);
		pthread_mutex_unlock (&lock_accept);
		if (sock >= 0)
		{
			int		left = 0;
			logger (DEBUG,"%04d\t worker_main: server: got connection from %s  %d\n",accsox->num, inet_ntoa (skaddr.sin_addr), sock);
			accsox->sock = sock;
			accsox->run_state = 0;
			accsox->time_out = first_Tout;
			sprintf(accsox->fromIP,"%s",inet_ntoa (skaddr.sin_addr));
			
			worker_loop((void*) accsox);
						 
		}
		else
		{
			logger (ERROR,"%04d\t worker_main: got connection error from %s  %d\n",accsox->num, inet_ntoa (skaddr.sin_addr), sock);
		}
		
		close (sock);
	}
	return NULL;
}



/*
 *	void *
 *	worker_loop (void *param)
 */
void *
worker_loop (void *param)
{
	int rv;
	ACCSOX*	accsox = (ACCSOX*) param;
	/////clinet 連接持續收資料直到錯誤或收到E01E才結束//////
	for(;;)
	{
		rv = accsox_proc (accsox);
		if (rv < 0)
		{
			break;
		}
		logger (INFO,"%04d\t accsox_proc: OK-\n", accsox->num);
	}

	logger(INFO,"%04d\t close OK-\n",accsox->num);
	//logger("%s\t close OK-\n",fromIP);

	return NULL;
}



/*
 *	int
 *	get_config ()
 */
 
/////////讀config的東西/////////
int get_config() {

	FILE *pFile;
	char line[300], keyStr[300];
	logger(WRITE,"\t\t get_config: Start Read config\n");

	
	
	pFile = fopen(config_path, "r");
	if (NULL == pFile) 
	{
		logger(ERROR,"\t\t get_config: open failure[%s],%s-\n",config_path,strerror(errno));
		return -1;

	}
	else 
	{

		while (fgets(line, 100, pFile) != NULL) {

			if (line[0] == '#')   ///#註解
					{
			} else {
				////get server port //
				if (strncmp(line, "Ser_Port=", 8) == 0) {
					memset(Server_Port, ' ', sizeof(Server_Port));
					strncpy(keyStr, strstr(line, "=") + 1, 10);
					if (strstr(keyStr, "\n") != NULL)
						memset(strstr(keyStr, "\n"), ' ', 1);
					sprintf(line, "Ser_Port : [%s]", keyStr);
					sprintf(Server_Port, "%s", keyStr);
					for(int i=0;i<10;i++)
					{
						if((int) Server_Port[i]==0 || Server_Port[i]=='\n')
							data_Server_Port[i]=' ';
						else
							data_Server_Port[i]=Server_Port[i];
					}
					logger(WRITE, "\t\t get_config: %s\n",  line);

				}
				
		 
				
				////get server_IP //
				else if (strncmp(line, "server_IP=", 9) == 0) {
					memset(server_IP, ' ', sizeof(server_IP));
					strncpy(keyStr, strstr(line, "=") + 1, 20);
					if (strstr(keyStr, "\n") != NULL)
						memset(strstr(keyStr, "\n"), ' ', 1);
					sprintf(line, "server_IP : [%s]", keyStr);
					logger(WRITE, "\t\t get_config: %s\n",  line);
					sprintf(server_IP, "%s", keyStr);
					
					
					for(int i=0;i<20;i++)
					{
						if((int) server_IP[i]==0 || server_IP[i]=='\n')
							data_server_IP[i]=' ';
						else
							data_server_IP[i]=server_IP[i];
					}
					
				}
				
				else if (strncmp(line, "Con_Sizq=", 8) == 0) {
					memset(Log_File, 0, sizeof(Log_File));
					strncpy(keyStr, strstr(line, "=") + 1, 20);
					if (strstr(keyStr, "\n") != NULL)
						memset(strstr(keyStr, "\n"), 0, 1);
					sprintf(line, "Con_Sizq : [%s]", keyStr);
					logger(WRITE, "\t\t get_config: %s\n",  line);
					Con_Sizq = atoi(keyStr);
				} 
				else if (strncmp(line, "first_Tout=", 10) == 0) {
					memset(Log_File, 0, sizeof(Log_File));
					strncpy(keyStr, strstr(line, "=") + 1, 20);
					if (strstr(keyStr, "\n") != NULL)
						memset(strstr(keyStr, "\n"), 0, 1);
					sprintf(line, "first_Tout : [%s]", keyStr);
					logger(WRITE,  "\t\t get_config: %s\n", line);
					first_Tout = atoi(keyStr);
				} 
				else if (strncmp(line, "write_log_s=", 11) == 0) {
					memset(Log_File, 0, sizeof(Log_File));
					strncpy(keyStr, strstr(line, "=") + 1, 20);
					if (strstr(keyStr, "\n") != NULL)
						memset(strstr(keyStr, "\n"), 0, 1);
					sprintf(line, "write_log_s : [%s]", keyStr);
					logger(WRITE,  "\t\t get_config: %s\n", line);
					write_log_s = atoi(keyStr);
				}

			}
		}
		fclose(pFile);
	}
	printf("get_config over\n");
	return 0;
}



/*
 *	void
 *	__accsox_intr (int signo)
 */
void
__accsox_intr (int signo)
{

	logger (3, "__accsox_intr: signo=%d-\n", signo);

	printf("__accsox_intr: signo=%d-\n", signo);
	switch (signo) {
	case SIGHUP:
	case SIGPIPE:
	case SIGALRM:
		logger (3, "__accsox_intr: SIGHUP/SIGPIPE/SIGALRM, signo=%d-\n", signo);
		/* NOOP */
		break;

	case SIGINT:
	case SIGQUIT:
	case SIGTERM:
		logger (3, "__accsox_intr: SIGINT/SIGQUIT/SIGTERM, signo=%d-\n", signo);
		Program_run = 0;
		//close (serv_accsox);
		exit (2);
		break;

	case SIGSEGV:
	case SIGBUS:
		logger (3, "__accsox_intr: SIGSEGV/SIGBUS, signo=%d-\n", signo);
		Program_run = 0;
		//close (serv_accsox);
		exit (2);
		break;

	default:
		break;
	}

	return;
}


