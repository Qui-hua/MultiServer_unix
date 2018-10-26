/*
 *	CHEN SEI-LIM
 *	2017-05-19
 */

#ifndef	_COMMON_H_
#define	_COMMON_H_


#define	MAX_LINE_LEN		10240

extern int sock0
  , Program_run    //程式是否執行 0停 1執行
  , client_num     //目前有多少連線
  ,	Con_Sizq       //連接數量
  , first_Tout     //預設time out
  , write_log_s    //log 等級 0全部不寫 1只寫錯誤 2寫錯誤 完成 3全寫 
  ,desc_logger
;

extern char Server_Port[10],data_Server_Port[10], server_IP[20],
		Log_File[256],data_server_IP[20],today_Date[8],config_path[256],rootfs[256];

extern char *			text_logger;


extern pthread_mutex_t	lock_logger;
extern pthread_mutex_t	lock_accept;

#ifdef __cplusplus   
extern "C" {
#endif	/* __cplusplus */


extern int			logger (int LOGCTL, char *fmt, ...);
extern void 		get_time(char *buf) ;

#ifdef __cplusplus
}
#endif	/* __cplusplus */


#endif	/* _COMMON_H_ */
