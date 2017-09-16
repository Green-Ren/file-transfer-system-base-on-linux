/*
 * client.h
 *
 *  Created on: Nov 29, 2013
 *  Author: Green-Ren
 */

# include <stdio.h>
# include <stdlib.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <sys/stat.h>
# include <errno.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/vfs.h>
# include <string.h>
# include <pthread.h>

# define port 8888

struct FilePackage
{
	char cmd;
    int ack;
    char userinfor[125];
    char filename[125];
    char buf[1024];
    char fname[50];
};

//打包函数
struct FilePackage pack(char tcmd, int tack, char *uname, char *tfilename, char *tbuf, char *fnme)
{
	struct FilePackage tPackage;

	tPackage.cmd = tcmd;
	tPackage.ack = tack;
	strcpy(tPackage.userinfor, uname);
	strcpy(tPackage.filename, tfilename);
	memcpy(tPackage.buf, tbuf,1024);
	strcpy(tPackage.fname, fnme);

	return tPackage;
}

