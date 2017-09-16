/*
 * server.h
 *
 *  Created on: Nov 29, 2013
 *  Author: Green-Ren
 */
# include <stdio.h>
# include <stdlib.h>
# include <errno.h>
# include <string.h>
# include <netdb.h>
# include <sys/stat.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <unistd.h>
# include <fcntl.h>
# include <arpa/inet.h>
# include <pthread.h>
# include <semaphore.h>
# include <sys/ipc.h>
# include <sys/vfs.h>
# include <dirent.h>
# include <netinet/in.h>
# include <sys/file.h>
# include <signal.h>

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

struct FilePackage pack(char tcmd, int tack, char *uname, char *fname, char *tbuf, char *fnme)
{
	struct FilePackage tpackage;
	tpackage.cmd = tcmd;
	tpackage.ack = tack;
	strcpy(tpackage.userinfor, uname);
	strcpy(tpackage.filename, fname);
	memcpy(tpackage.buf, tbuf, 1024);
	strcpy(tpackage.fname, fnme);
	return tpackage;
}

