/*
 * client.c
 *
 *  Created on: Nov 29, 2013
 *  Author: Green-Ren
 */

# include "client.h"

//函数声明
int connectto(int argc, char *argv[]);
void logo(void);
int login(void);
void update(void);
void download(void);
void show(void);
void menu(void);
void sock(void);

//全局变量
char buffer[1024];
struct sockaddr_in sockaddr1;
char user[1024];
struct FilePackage data;
int clientfd;
char ipaddr[50];
char username[20];


int main(int argc, char *argv[])
{
	//char *pbuf = buffer;
	strcpy(ipaddr, argv[1]);
	int flag;
	if((connectto(argc, argv)) != 0)
	{
		exit(0);
	}
	else
		printf("connect success!!\n");

	logo();

	login();

	menu();



	return 0;
}


//显示登录logo
void logo()
{

	printf("\n\33[32m*******************   Welcome to File Storage System  *******************\33[0m");
	printf("\n\33[31m                        *******         *********                       \33[0m");
	printf("\n\33[31m                        **              **                              \33[0m");
	printf("\n\33[31m                        *******         *********                       \33[0m");
	printf("\n\33[31m                        **                     **                       \33[0m");
	printf("\n\33[31m                        **              *********                       \33[0m");
	printf("\n\33[32m************************************************************************\33[0m\n");

	return;
}


//请求连接
int connectto(int argc, char *argv[])
{
	int i, count = 0;
	if(argc != 2)
	{
		printf("format error:you must enter IP like:client 192.168.142.22\n");
		exit(0);
	}
	for(i=0; *(argv[1]+i)!='\0'; i++)
	{
		if(*(argv[1]+i)=='.')
			count++;
	}
	if(count != 3)
	{
		printf("IP format error\n");
		exit(0);
	}

	sock();

	return 0;
}

void sock()
{
	int nbytes;

	//建立socket套接字
	if((clientfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket:");
		printf("\n");
		exit(0);
	}

	memset(&sockaddr1, 0, sizeof(sockaddr1));//初始化
	sockaddr1.sin_family = AF_INET; //IP
	sockaddr1.sin_addr.s_addr = inet_addr(ipaddr);//IP地址
	sockaddr1.sin_port = htons(port);//端口号

	if(connect(clientfd, (struct sockaddr*) &sockaddr1, sizeof(sockaddr1)) == -1)
	{
		perror("connect：");
		printf("\n");
		exit(1);
	}

	//发送数据
	data = pack(' ', 0, "", "", "", "");
	if((nbytes = write(clientfd, &data, sizeof(struct FilePackage))) == -1)
	{
		fprintf(stderr, "write: %s\n", strerror(errno));
		exit(1);
	}

	//读取服务端发来的登录验证信息
	if((nbytes = recv(clientfd, &data, sizeof(struct FilePackage), 0)) == -1)
	{
		fprintf(stderr, "recv: %s\n", strerror(errno));
		exit(1);
	}
	strncpy(user, data.userinfor, 1024);
	close(clientfd);
}

int login()
{
	char *puser = user;
	int flag = 1, nflag = 1;   //1表示验证失败，0表示验证成功
	//char username[20];
	char userpwd[20];
	char UN[20];
	char UP[20];
	int i = 0;

	while(flag != 0)
		{
			printf("\nclientId:");
			scanf("%s", username);
			printf("PassWord:");
			scanf("%s", userpwd);

			while(nflag != 0)  //检测用户名是否存在，存储格式regry*123#
			{
				while(*puser != '*' && (*puser) != '\0')
				{
					if(i < 20)
					{
						UN[i] = *puser;
						++i;
						++puser;
					}
				}
				++puser;          //跳过＃
				i = 0;
				while(*puser != '#' && (*puser) != '\0')
				{
					if(i < 20)
					{
						UP[i] = *puser;
						++i;
						++puser;
					}
				}
				++puser;
				if(strcmp(UN, username) == 0)
				{
					nflag = 0;
					if(strcmp(UP, userpwd) == 0)
					{
						flag = 0;
						printf("\nlogin success!\n");
						break;
					}
					else
					{
						nflag = 1;
						memset(UP, '\0', 20);
						memset(UN, '\0', 20);
						puser = user;
						printf("\n Admin name or password is error! \n");
						i = 0;
						break;
					}
				}
				else if(*puser == '\0')
				{
					puser = user;
					i = 0;
					memset(UP, '\0', 20);
					memset(UN, '\0', 20);
					printf("\n user name or password is error!\n");
					break;
				}

				i = 0;
				memset(UP, '\0', 20);
				memset(UN, '\0', 20);
			}

		}
	return 0;
}

void menu()
{
	int num;
	void (*p)(void) = &show;
	//p = &show;
L:
	system("clear");
P:
	printf("\n\33[32m*******************  Please Choose Your Number  ***********************\33[0m");
	printf("\n\33[31m                       1. Update File                                  \33[0m");
	printf("\n\33[31m                       2. Download File                                \33[0m");
	printf("\n\33[31m                       3. Exit                                         \33[0m");
	printf("\n\33[32m***********************************************************************\33[0m");


	printf("\n\nenter your number:");
	scanf("%d", &num);

	switch(num)
	{
	case 1:
	{
		(*p)();
		update();
		(*p)();
		goto P;
	}
	case 2:
	{
		(*p)();
		download();
		(*p)();
		goto P;
	}
	case 3:
	{
		exit(0);
	}

	default:
	{
		printf("\nplease enter the correct num!\n");
		goto L;
	}

	}

	return;
}

//上传文件
void update()
{

	int openfd;
	int nbytes;
	char filen[50];
	char fname[50];
	printf("please enter your updated filename:\n");
	scanf("%s", filen);
	strcpy(fname, "./");
	strcat(fname, filen);

	if((openfd = open(fname, O_RDONLY)) == -1)
	{
		perror("open:");
		exit(1);
	}

	if((read(openfd, buffer, 1024) == -1))
	{
		perror("read:");
		exit(1);
	}

	close(openfd);

	//建立socket套接字
	if((clientfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket:");
		printf("\n");
		exit(0);
	}

	memset(&sockaddr1, 0, sizeof(sockaddr1));//初始化
	sockaddr1.sin_family = AF_INET; //IP
	sockaddr1.sin_addr.s_addr = inet_addr(ipaddr);//IP地址
	sockaddr1.sin_port = htons(port);//端口号

	if(connect(clientfd, (struct sockaddr*) &sockaddr1, sizeof(sockaddr1)) == -1)
	{
		perror("connect：");
		printf("\n");
		exit(1);
	}

	//发送数据
	data = pack('U', 0, "", "", buffer, filen);
	if((nbytes = write(clientfd, &data, sizeof(struct FilePackage))) == -1)
	{
		fprintf(stderr, "write: %s\n", strerror(errno));
		exit(1);
	}

	if((nbytes = recv(clientfd, &data, sizeof(struct FilePackage), 0)) == -1)
	{
		fprintf(stderr, "recv: %s\n", strerror(errno));
		exit(1);
	}

	close(clientfd);

	return;
}


//下载文件
void download()
{
 	int openfd;
	int nbytes;
	char filen[50];
	char fname[50];
	char clientf[50];
	printf("please enter your download filename:\n");
	scanf("%s", filen);
	strcpy(fname, "./");
	strcat(fname, username);
	strcat(fname, "/");
	strcat(fname, filen);

	printf("fname: %s", fname);

	//建立socket套接字
	if((clientfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket:");
		printf("\n");
		exit(0);
	}

	memset(&sockaddr1, 0, sizeof(sockaddr1));//初始化
	sockaddr1.sin_family = AF_INET; //IP
	sockaddr1.sin_addr.s_addr = inet_addr(ipaddr);//IP地址
	sockaddr1.sin_port = htons(port);//端口号

	if(connect(clientfd, (struct sockaddr*) &sockaddr1, sizeof(sockaddr1)) == -1)
	{
		perror("connect：");
		printf("\n");
		exit(1);
	}

	//发送数据
	data = pack('D', 0, "", "", buffer, fname);
	if((nbytes = write(clientfd, &data, sizeof(struct FilePackage))) == -1)
	{
		fprintf(stderr, "write: %s\n", strerror(errno));
		exit(1);
	}

	if((nbytes = recv(clientfd, &data, sizeof(struct FilePackage), 0)) == -1)
	{
		fprintf(stderr, "recv: %s\n", strerror(errno));
		exit(1);
	}

	strcpy(clientf, "./");
	strcat(clientf, filen);
	if((openfd = open(clientf, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU)) == -1)
	{
		perror("open error:");
		exit(1);
	}
	printf("this is writing\n");
	//strcpy(buffer, data.buf);
	//printf("data.buf:%s\n", data.buf);
	if((write(openfd, data.buf, 1024)) == -1)
	{
		perror("write error:");
		exit(1);
	}

	close(openfd);

	close(clientfd);

	return;
}

//显示文件列表
void show()
{
	//char temp[10];
	//strcpy(temp, "clear");
	//system("clear");

	printf("\n\33[32m*********************client files list: *********************\33[0m\n");
	//strcpy(temp, "ls");
	system("ls");

	printf("\n\33[32m********************server files list: **********************\33[0m\n");
	printf("%s\n", data.filename);

	return;
}











