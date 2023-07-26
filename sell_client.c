#include "sell.h"

int sockfd;
int addrlen;
struct sockaddr_in  server_addr; 
int userID = 0;
pthread_t pid;
int react;

int login_f =  -1;
	
int generateRandomFourDigit() {
    return rand() % 10000; // 取rand()的结果对10000取模，确保得到一个四位数的随机数字
}

void put(int sockfd)
{
	RequestPack pack;
	int sel;
	int num;

	srand(time(NULL));

	printf("存放物品为：\n");
	printf("\t 1 快递 \n");
	printf("\t 2 外卖 \n");
	scanf("%d", &sel);

	if(sel == 1)
	{
		pack.type = REQUEST_PUT_EXPRESS;
	}
	else if(sel == 2)
	{
		pack.type = REQUEST_PUT_FOOD;
	}
	else
	{
		printf("valid select\n");
		return;
	}

	num = generateRandomFourDigit();
	pack.Num = num;

	// printf("取件码为：%d\n", num);

	pack.userID = userID++;


	send(sockfd, &pack, sizeof(RequestPack), 0);
	// printf("send ok\n");
	recv(sockfd, &react, sizeof(int), 0);
	// printf("recv seccess\n");
	if(react == 1)
	{
		printf("存件成功，取件码为：%d\n\n", num);
	}
	else
	{
		printf("存件失败\n\n");
	}
	return;
}

void get(int sockfd)
{
	RequestPack pack;
	int sel;
	int num;

	printf("取出物品为：\n");
	printf("\t 1 快递 \n");
	printf("\t 2 外卖 \n");
	scanf("%d", &sel);

	if(sel == 1)
	{
		pack.type = REQUEST_GET_EXPRESS;
	}
	else if(sel == 2)
	{
		pack.type = REQUEST_GET_FOOD;
	}
	else
	{
		printf("valid select\n");
		return;
	}

	printf("请输入取件码：\n");
	scanf("%4d", &num);

	pack.Num = num;
	pack.userID = userID++;

	send(sockfd, &pack, sizeof(RequestPack), 0);
	recv(sockfd, &react, sizeof(int), 0);
	// printf("recv seccess\n");
	printf("取件%s\n\n", react == 1 ? "成功":"失败");
}

void buy(int sockfd)
{	
	RequestPack pack;
	int sel;

	printf("\t 1 补货 \n");
	printf("\t 2 购货 \n");
	scanf("%d", &sel);

	pack.type = REQUEST_VENDING;
	pack.userID = userID++;

	if(sel == 1)
	{
		pack.Num = 0;
		send(sockfd, &pack, sizeof(RequestPack), 0);
		recv(sockfd, &react, sizeof(int), 0);
		// printf("recv seccess\n");
		printf("补货%s\n\n", react == 1 ? "成功":"失败");
		return;
	}
	else if(sel == 2)
	{
		pack.Num = 1;
		send(sockfd, &pack, sizeof(RequestPack), 0);
		recv(sockfd, &react, sizeof(int), 0);
		// printf("recv seccess\n");
		printf("购货%s\n\n", react == 1 ? "成功":"失败");		
		return;
	}
	else
	{
		printf("valid select\n");
	}
	return;


}

int main(int argc, char **argv)
{
	int sel;
	int ret; 
	int min_sel,max_sel;
	int portnumber;
	
	RequestPack pack;
	
	
	if(argc<3)
	{
		printf("cmd: %s ip portnumber\n",argv[0]);
		return -1;
	}
	//argv2 存放的是端口号 ，读取该端口，转换成整型变量
	if((portnumber=atoi(argv[2]))<0)
	{
		fprintf(stderr,"Usage:%s hostname portnumber\a\n",argv[0]);
		exit(1);
	}
	sockfd = socket(PF_INET,SOCK_STREAM, 0);	
	if(sockfd<0)
	{
		perror("socket() fail\n");
		return -1;
	}
	
	server_addr.sin_family =  PF_INET;	
	server_addr.sin_port   =  htons(portnumber);
	server_addr.sin_addr.s_addr   =  inet_addr(argv[1]);
	
	addrlen = sizeof(struct sockaddr_in);
	
	ret = connect(sockfd, (struct sockaddr* )&server_addr, addrlen);
	if(ret == -1)
	{
		printf("connect server err\n");
		return -1;
	}
	
	while(1)
	{
		//getchar();
		system("clear");
		printf("\t 1 存放物品\n");
		printf("\t 2 取出物品\n");
		printf("\t 3 售货柜\n");
		printf("\t 0 退出\n");
		
		
		fflush(stdin);

		scanf("%d",&sel);
		if(sel == 0)
		{
			break;
		}

		min_sel = 1;
		max_sel = 3;

		
		if(sel<min_sel || sel > max_sel)
		{
			printf("Valid choice ,try again\n");
			continue;
		}
		switch(sel)
		{
			case 1:
				put(sockfd);
				break;
			case 2:
				get(sockfd);
				break;
			case 3:
				buy(sockfd);
				break;
			default:
				break;
		}
		getchar();
		getchar();

	}
}

