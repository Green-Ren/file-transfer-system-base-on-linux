/*
 * server.c
 *
 *  Created on: Nov 29, 2013
 *  Author: Green-Ren
 */
# include "server.h"
# define Max_thread 5
//全局变量
char Admin[1024];
char User[1024];
int serverfd, clientfd, sin_size;
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
int nbytes;
char buffer[1024];
char filelist[1024];
struct FilePackage data;
//

//函数声明
void Init_admin_user();
int menu();
void getlist(char *username);
void thread_function(void);

int main(int argc, char *argv[])
{


	Init_admin_user();
	int menu_rnum;

L:
	printf("\n\33[32m*******************   Welcome to File Storage System  *******************\33[0m");
	printf("\n\33[31m                        *******         *********                       \33[0m");
	printf("\n\33[31m                        **              **                              \33[0m");
	printf("\n\33[31m                        *******         *********                       \33[0m");
	printf("\n\33[31m                        **                     **                       \33[0m");
	printf("\n\33[31m                        **              *********                       \33[0m");
	printf("\n\33[32m************************************************************************\33[0m\n");

	int flag = 1, nflag = 1;   //登录成功则flag为0，失败为1；
	char Adminname[20] = "\0";
	char Adminpwd[20] = "\0";

	int i = 0;
	char AN[20] = "\0";
	char AP[20] = "\0";
	char *padmin = Admin;


	while(flag != 0)
	{
		printf("\nAdmin Id:");
		scanf("%s", Adminname);
		printf("Admin Pwd:");
		scanf("%s", Adminpwd);

		while(nflag != 0)  //检测用户名是否存在,存储格式：regry*123#
		{
			while(*padmin != '*' && (*padmin) != '\0')
			{
				if(i < 20)
				{
					AN[i] = *padmin;
					++i;
					++padmin;
				}
			}
			++padmin;          //跳过*
			i = 0;
			while(*padmin != '#' && (*padmin) != '\0')
			{
				if(i < 20)
				{
					AP[i] = *padmin;
					++i;
					++padmin;
				}
			}
			++padmin;       //跳过＃
			if(strcmp(AN, Adminname) == 0)
			{
				nflag = 0;
				if(strcmp(AP, Adminpwd) == 0)
				{
					flag = 0;
					printf("\nlogin success!\n");
					break;
				}
				else
				{
					nflag = 1;
					memset(AP, '\0', 20);
					memset(AN, '\0', 20);
					padmin = Admin;
					printf("\n Admin name or password is error! \n");
					i = 0;
					break;
				}
			}
			else if(*padmin == '\0')
			{
				padmin = Admin;
				i = 0;
				memset(AP, '\0', 20);
				memset(AN, '\0', 20);
				printf("\n Admin name or password is error!\n");
				break;
			}

			i = 0;
			memset(AP, '\0', 20);
			memset(AN, '\0', 20);
		}

	}//while end


	if((menu_rnum = menu()) == 1)
	{
		printf("\nLogout success!\n");
		goto L;
	}

	return 0;
}

void Init_admin_user()
{
	int nread;
	int fd;
	char *padmin = Admin;
	char *puser = User;
	int nleft = 1024;

	//从admin.txt中读取管理员信息到数组
	if((fd = open("./admin.txt", O_RDONLY)) < 0)
	{
		printf("can not open admin.txt!");
	}

	while(nleft>0)
	{
		if((nread = read(fd, padmin, 20)) < 0)
		{
			printf("can not read from admin.txt!");
		}
		else
			if(nread == 0)
				break;
		padmin += nread;  //指针向后移动nread
		nleft -= nread;
	}
	close(fd);

	//从user.txt中读取用户信息到数组
	if((fd = open("./user.txt", O_RDONLY)) < 0)
		{
			printf("can not open user.txt!");
		}
	while(nleft > 0)
	{
		if((nread = read(fd, puser, 20)) < 0)
		{
			printf("can not read from user.txt!");
		}
		else if(nread == 0)
			break;
		puser += nread;
		nleft -= nread;
	}
	close(fd);
}


int  menu()
{
	int num;
	//char temp[10];
LP:
	system("clear");

	printf("\n\33[32m*******************  Please Choose Your Number  ***********************\33[0m");
	printf("\n\33[31m                       1. Run server                                   \33[0m");
	printf("\n\33[31m                       2. Log out                                      \33[0m");
	printf("\n\33[31m                       3. Exit server                                  \33[0m");
	printf("\n\33[32m***********************************************************************\33[0m");

	printf("\n\n");
	printf("enter your number:");
	scanf("%d", &num);
	//printf("%d", num);

	switch(num)
	{
	case 1:
	{
		//建立套接字
		if((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			fprintf(stderr, "socket error :%s", strerror(errno));
			exit(1);
		}

		//进行初始化并，并定义套接字地址
		bzero(&server_addr, sizeof(struct sockaddr_in));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(port);
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

		//命名void *thread_function(void *arg);
		if(bind(serverfd, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr)) < 0)
		{
			fprintf(stderr, "bind error: %s\n", strerror(errno));
			exit(1);
		}
		//建立套接字队列
		if(listen(serverfd, 20) == -1)
		{
			fprintf(stderr, "listen error: %s\n", strerror(errno));
			exit(1);
		}

		//使用多线程取代循环****************************************************************
		int n, i;
		pthread_t thread[Max_thread];

		for(i = 0; i < Max_thread; i++)
		{
			n = pthread_create(&thread[i], NULL, (void *)thread_function, NULL);//创建了一个线程
			if(n != 0)
			{
				printf("pthread_create error\n");
				exit(1);
			}
			sleep(1);
		}

		printf("waiting for threads exit!!\n");

		for(i = 0; i < Max_thread; i++)
		{
			n = pthread_join(thread[i], NULL);
			if(n == 0)
			{
				printf("the thread  exit !\n", i);
				pthread_create(&thread[i], NULL, (void *)thread_function, NULL);//创建了一个线程
				//printf("the thread exit !\n");
			}
			else
			{
				perror("the thread exit error!\n");
			}
		}

		printf("all thread exit!");
		//***********************************************************************************************
		/*
		while(1)
		{
			sin_size = sizeof(struct sockaddr_in);

			printf("\nserver is running\n");
			//接收连接
			if((clientfd = accept(serverfd, (struct sockaddr *) (&client_addr), &sin_size)) == -1)
			{
				fprintf(stderr, "accept error: %s\n", strerror(errno));
				exit(1);
			}

			//printf("\nserver is running\n");
			fprintf(stderr, "\nserver get connection from %s\n", inet_ntoa(client_addr.sin_addr));

			if((nbytes = read(clientfd, &data, sizeof(struct FilePackage))) == -1)
			{
				fprintf(stderr, "read error: %s\n", strerror(errno));
				exit(1);
			}

			//printf("fname:%s\n", data.fname);

			// 文件上传
	        ///******************************************************************************************
			if((data.cmd) == 'U')
			{
				//printf("this is U\n");
				//FILE *fp;
				int openfd;
				char dir[125];
				strcpy(dir, "./regry/");
				strcat(dir,data.fname);
				//printf("%s\n", dir);
				//if((fp = fopen(dir, "w")) == NULL)
				if((openfd = open(dir, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU)) == -1)
				{
					perror("open error:");
					exit(1);
				}
				//printf("this is writing\n");
				//strcpy(buffer, data.buf);
				//printf("data.buf:%s\n", data.buf);
				if((write(openfd, data.buf, 1024)) == -1)
				{
					perror("write error:");
					exit(1);
				}

				close(openfd);
			}
		     //**********************************************************************************************

			//文件下载
			//************************************************************************************************
			if((data.cmd) == 'D')

			{
				//printf("this is D\n");
				int openfd;
				if((openfd = open(data.fname, O_RDONLY)) == -1)
				{
					perror("open error:");
					exit(1);
				}
				//printf("this is writing\n");
				if((read(openfd, buffer, 1024)) == -1)
				{
					perror("write error:");
					exit(1);
				}

				close(openfd);
			}
			 //*******************************************************************************************

			strcpy(temp, "regry");
			getlist(temp);

			//打包数据后发送到客户端
			data = pack(' ', 0, User, filelist, buffer, "");
			if((nbytes = send(clientfd, &data, sizeof(struct FilePackage), 0)) == -1)
			{
				fprintf(stderr, "write error: %s\n", strerror(errno));
				exit(1);
			}

			//此次通讯结束
			close(clientfd);
			//循环下一个
		}*/

		close(serverfd);
		exit(0);
	}
	case 2:
	{
		return 1;
	}
	case 3:
	{
		exit(1);
	}
	default:
	{
		printf("Your Number is wrong!!\n");
		goto LP;
	}
	}

}

void getlist(char *username)
{
	DIR *pdir;
	struct dirent *pent;
	char dffdir[60] = "\0";
	strcpy(filelist, "");
	strcat(dffdir, "./");
	strcat(dffdir, username);
	if((pdir = opendir(dffdir)) == NULL)
	{
		fprintf(stderr, "open dir error\n");
		return;
	}
	while(1)
	{
		pent = readdir(pdir);
		if(pent == NULL)
		{
			break;
		}
		else
		{
			strcat(filelist, pent->d_name);
			strcat(filelist, "  ");
		}
	}

	closedir(pdir);
}



void thread_function()
{
	char temp[10];
	sin_size = sizeof(struct sockaddr_in);

	printf("\nserver is running\n");
	//接收连接
	if((clientfd = accept(serverfd, (struct sockaddr *) (&client_addr), &sin_size)) == -1)
	{
		fprintf(stderr, "accept error: %s\n", strerror(errno));
		exit(1);
	}

	//printf("\nserver is running\n");
	fprintf(stderr, "\nserver get connection from %s\n", inet_ntoa(client_addr.sin_addr));

	if((nbytes = read(clientfd, &data, sizeof(struct FilePackage))) == -1)
	{
		fprintf(stderr, "read error: %s\n", strerror(errno));
		exit(1);
	}

	//printf("fname:%s\n", data.fname);

	// 文件上传
	///******************************************************************************************
	if((data.cmd) == 'U')
	{
		//printf("this is U\n");
		//FILE *fp;
		int openfd;
		char dir[125];
		strcpy(dir, "./regry/");
		strcat(dir,data.fname);
		//printf("%s\n", dir);
		//if((fp = fopen(dir, "w")) == NULL)
		if((openfd = open(dir, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU)) == -1)
		{
			perror("open error:");
			exit(1);
		}
		//printf("this is writing\n");
		//strcpy(buffer, data.buf);
		//printf("data.buf:%s\n", data.buf);
		if((write(openfd, data.buf, 1024)) == -1)
		{
			perror("write error:");
			exit(1);
		}

		close(openfd);
	}
	 //**********************************************************************************************

	//文件下载
	//************************************************************************************************
	if((data.cmd) == 'D')

	{
		//printf("this is D\n");
		int openfd;
		if((openfd = open(data.fname, O_RDONLY)) == -1)
		{
			perror("open error:");
			exit(1);
		}
		//printf("this is writing\n");
		if((read(openfd, buffer, 1024)) == -1)
		{
			perror("write error:");
			exit(1);
		}

		close(openfd);
	}
	 //*******************************************************************************************

	strcpy(temp, "regry");
	getlist(temp);

	//打包数据后发送到客户端
	data = pack(' ', 0, User, filelist, buffer, "");
	if((nbytes = send(clientfd, &data, sizeof(struct FilePackage), 0)) == -1)
	{
		fprintf(stderr, "write error: %s\n", strerror(errno));
		exit(1);
	}

	//此次通讯结束
	close(clientfd);
}






















