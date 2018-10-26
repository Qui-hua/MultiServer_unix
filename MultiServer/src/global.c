

#include <accsox.h>
#include <config.h> 
#include <global.h> 



EXPORT ACCSOX *				pool_accsox = NULL;
EXPORT pthread_mutex_t *	lock_cobol_data=NULL;
/*
 *	int
 *	logger (const char *fmt, ...)
 */
int
logger (int LOGCTL, char *fmt,...)
{
	va_list	ap;
	struct	tm tmv;
	struct	timeval tv;
	struct	timezone tz;
	char *	text = text_logger;
	int		desc = desc_logger;
	int		left = 0;
	struct	flock lock;


	if (desc == -1) return -1;
	

	gettimeofday (&tv, &tz);
	localtime_r (&tv.tv_sec, &tmv);
	
	// 1. 取得線程鎖
	pthread_mutex_lock (&lock_logger);
	
	left = sprintf (text, "%04d-%02d-%02d %02d:%02d:%02d.%03ld  0x%08X ",
		tmv.tm_year + 1900,
		tmv.tm_mon + 1,
		tmv.tm_mday,
		tmv.tm_hour,
		tmv.tm_min,
		tmv.tm_sec,
		tv.tv_usec/1000,
		(int) pthread_self ()
	);
	va_start (ap, fmt);
	left += vsnprintf (&text[left], MAX_LINE_LEN, fmt, ap);
	va_end (ap);
	text[left] = '\0';
	
	write_log_s;
	if (LOGCTL <=write_log_s)   ///完全不紀錄
	{
		write (desc, text, left);
	} 
	
	
	// 5. 釋放線程鎖
	pthread_mutex_unlock (&lock_logger);

	return 0;
}



/*
 *	void
 *	get_time(char *buf)
 */
////取得當前時間//
void 
get_time(char *buf) 
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	struct timeval tv;
	gettimeofday(&tv, NULL);
	memset(buf, 0, sizeof(buf));
	int year = tm.tm_year + 1900, mon = tm.tm_mon + 1, d = tm.tm_mday, 
		h =	tm.tm_hour, m = tm.tm_min, s = tm.tm_sec, ms = tv.tv_usec / 1000;
	sprintf(buf, "%04d%02d%02d%02d%02d%02d%03d\t", year, mon, d, h, m, s, ms);
}


