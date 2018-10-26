/*
log位置帶參數
 */

#include <config.h>
#include <accsox.h> 
#include <global.h> 


int
main (int argc, char **argv, char **envp)
{

	if (argc < 1) {
		printf("usage: <rootfs>  \n");
		exit (0);
	} 	
	memset (config_path, 0x0, sizeof (char) * 256);
	memset (rootfs, 0x0, sizeof (char) * 256);
	memset (Log_File, 0x0, sizeof (char) * 256);
	sprintf( rootfs, "%s",(argv[1]));
	sprintf( Log_File, "%s/log/accsox.log",(argv[1]) );
	sprintf( config_path, "%s/config/accsox_config",(argv[1]));
	
	printf("Log_File %s\n",Log_File);
	printf("config_path %s\n",config_path);
	
	accsox_init ();    ///初始化
	accsox_main ();	   ///主程式
	return 0;
}

