#include "sell.h"

GoodsInfo GOODS[MAX_GOODS_NUM];
sem_t *Sem_express;
sem_t * Sem_food;
sem_t * Sem_vending;
sem_t *Sem_express_act;
sem_t *Sem_food_act;
sem_t *Sem_vending_act;

pid_t Express_pid;
pid_t Food_pid;
pid_t Vending_pid;
pthread_t Express_tid;
pthread_t Food_tid;
pthread_t Vending_tid;
pthread_t Accept_tid;

int pipe_fd[2];
int pipe_act_fd[2];



void GOODS_Init()
{
    int i;

    for(i = 0; i < MAX_GOODS_NUM; i++)
    {
        if(i < 5)
        {
            GOODS[i].type   = GOODS_EXPRESS;
            GOODS[i].status = 0;
            GOODS[i].num    = -1;
        }
        else if(i < 8)
        {
            GOODS[i].type   = GOODS_FOOD;
            GOODS[i].status = 0;
            GOODS[i].num    = -1;
        }
        else
        {
            GOODS[i].type   = GOODS_VENDING;
            GOODS[i].status = 1;
            GOODS[i].num    = -1;
        }
    }
}


void *express()
{
    int i;
    int react;

    printf("express thread start \n");
    RequestPack pack;
    read(pipe_fd[0], &pack, sizeof(RequestPack));
    // close(pipe_fd[0]);

    if(pack.type == REQUEST_PUT_EXPRESS)
    {
        for(i = 0; i < 5; i++)
        {
            if(GOODS[i].status == 0)
            {
                GOODS[i].status = 1;
                GOODS[i].num = pack.Num;
                printf("存件成功,柜号 %d  取件码: %d\n", i , pack.Num);
                react = 1;
                printf("111");
                write(pipe_act_fd[1], &react, sizeof(int));
                sem_post(Sem_express_act);
                return NULL;
            }
        }
        printf("快递柜已满\n");
        react = 0;
        write(pipe_act_fd[1], &react, sizeof(int));
        sem_post(Sem_express_act);
        return NULL;
    }
    else
    {
        for(i = 0; i < 5; i++)
        {
            if(GOODS[i].status == 0)
                continue;
            else if(GOODS[i].num == pack.Num)
            {
                printf("取件码正确，请取件\n");
                GOODS[i].status = 0;
                react = 1;
                write(pipe_act_fd[1], &react, sizeof(int));
                sem_post(Sem_express_act);
                return NULL;
            }
        }
        printf("取件码错误，请重试\n");
        react = 0;
        write(pipe_act_fd[1], &react, sizeof(int));
        sem_post(Sem_express_act);
        return NULL;
    }
}

void *food()
{
    int i;
    int react;
    RequestPack pack;
    close(pipe_fd[1]);
    read(pipe_fd[0], &pack, sizeof(RequestPack));
    // printf("pack Num is %d\n", pack_get.Num);

    if(pack.type == REQUEST_PUT_FOOD)
    {
        for(i = 5; i < 8; i++)
        {
            if(GOODS[i].status == 0)
            {
                GOODS[i].status = 1;
                GOODS[i].num = pack.Num;
                printf("存件成功,柜号 %d   取件码: %d\n", i , pack.Num);
                react = 1;
                write(pipe_act_fd[1], &react, sizeof(int));
                sem_post(Sem_food_act);
                return NULL;
            }
        }
        printf("外卖柜已满\n");
        react = 0;
        write(pipe_act_fd[1], &react, sizeof(int));
        sem_post(Sem_food_act);
        return NULL;
    }
    else
    {
        for(i = 5; i < 8; i++)
        {
            if(GOODS[i].status == 0)
                continue;
            else if(GOODS[i].num == pack.Num)
            {
                printf("取件码正确，请取件\n");
                GOODS[i].status = 0;
                react = 1;
                write(pipe_act_fd[1], &react, sizeof(int));
                sem_post(Sem_food_act);
                return NULL;            
            }
        }
        printf("取件码错误，请重试\n");
        react = 0;
        write(pipe_act_fd[1], &react, sizeof(int));
        sem_post(Sem_food_act);
        return NULL;
    }
}

void *vending()
{
    int i;
    int react;
    RequestPack pack;
    close(pipe_fd[1]);
    read(pipe_fd[0], &pack, sizeof(RequestPack));

    if(pack.Num == 0)
    {
        for(i = 8; i < MAX_GOODS_NUM; i++)
        {
            if(GOODS[i].status == 1)
                continue;
            else
            {
                GOODS[i].status = 1;
                printf("补货成功\n");
                react = 1;
                write(pipe_act_fd[1], &react, sizeof(int));
                sem_post(Sem_vending_act);
                return NULL;
            }
        }
        printf("无需补货\n");
        react = 0;
        write(pipe_act_fd[1], &react, sizeof(int));
        sem_post(Sem_vending_act);        
        return NULL;
    }
    else
    {
        for(i = 8; i < MAX_GOODS_NUM; i++)
        {
            if(GOODS[i].status == 0)
                continue;
            else
            {
                GOODS[i].status = 0;
                printf("购买成功\n");
                react = 1;
                write(pipe_act_fd[1], &react, sizeof(int));
                sem_post(Sem_vending_act);
                return NULL;            
            }
        }
        printf("缺货中\n");
        react = 0;
        write(pipe_act_fd[1], &react, sizeof(int));
        sem_post(Sem_vending_act);        
        return NULL;    
    }
}


void *accept_thread_func(void *arg)
{
    int *clientfd = (int *)arg;
    int len;
    int react;
    RequestPack pack;

    while(1)
    {
        len = recv(*clientfd, &pack, sizeof(RequestPack), 0);
        if(len <= 0)
        {
            printf("pack recv err\n");
            return NULL;
        }
        printf("pack Num is %d\n", pack.Num);
        // pthread_mutex_lock(&mutex);
        // msgbuf.Pack = pack;
        // msgbuf.index = number++;
        // msgsnd(msgid, &pack, sizeof(RequestPack), 0);
        // pthread_mutex_unlock(&mutex);
        // printf("msg send to queue\n");
        close(pipe_fd[0]);
        // close(pipe_act_fd[1]);

        switch(pack.type)
        {
            case REQUEST_NONE:
                printf("PACK TYPE ERR\n");
                break;

            case REQUEST_PUT_EXPRESS:
                write(pipe_fd[1], &pack, sizeof(RequestPack));
                // printf("write pack to pipe\n");
                sem_post(Sem_express);

                sem_wait(Sem_express_act);
                printf("send pack to pipe\n");

                read(pipe_act_fd[0], &react, sizeof(int));
                send(*clientfd, &react, sizeof(int), 0);
                break;

            case REQUEST_GET_EXPRESS:
                sem_post(Sem_express);
                write(pipe_fd[1], &pack, sizeof(RequestPack));

                sem_wait(Sem_express_act);
                read(pipe_act_fd[0], &react, sizeof(int));
                send(*clientfd, &react, sizeof(int), 0);
                break;

            case REQUEST_PUT_FOOD:
                // printf("send pack to pipe\n");
                sem_post(Sem_food);
                write(pipe_fd[1], &pack, sizeof(RequestPack));

                sem_wait(Sem_food_act);
                read(pipe_act_fd[0], &react, sizeof(int));
                send(*clientfd, &react, sizeof(int), 0);                
                break;

            case REQUEST_GET_FOOD:
                sem_post(Sem_food);
                write(pipe_fd[1], &pack, sizeof(RequestPack));
                
                sem_wait(Sem_food_act);
                read(pipe_act_fd[0], &react, sizeof(int));
                send(*clientfd, &react, sizeof(int), 0);
                break;

            case REQUEST_VENDING:
                sem_post(Sem_vending);
                write(pipe_fd[1], &pack, sizeof(RequestPack));
                
                sem_wait(Sem_vending_act);
                read(pipe_act_fd[0], &react, sizeof(int));
                send(*clientfd, &react, sizeof(int), 0);
                break;

        }
    }
}

int main(int argc, char **argv)
{
    int lsfd,newfd;
	int addrLen;
    socklen_t cli_addr_len;
	struct sockaddr_in   my_addr; 
	struct sockaddr_in   cli_adr;	
	char buf[64]="xuezhiqian fuhele\n";
	int *arg;
	int i;
	int portnumber;
    RequestPack request_pack;

    if(argc<2)
	{
		printf("cmd: %s  portnumber\n",argv[0]);
	 	return -1;
	}
    if((portnumber=atoi(argv[1]))<0)
	{
	 	fprintf(stderr,"Usage:%s portnumber\a\n",argv[0]);
	 	exit(1);
	}	



    Sem_express = sem_open("/Sem_express", O_CREAT, 0666, 1);
    Sem_food = sem_open("/Sem_food", O_CREAT, 0666, 1);
    Sem_vending = sem_open("/Sem_vending", O_CREAT, 0666, 1);
    Sem_express_act = sem_open("/Sem_express_act", O_CREAT, 0666, 1);
    Sem_food_act = sem_open("/Sem_food_act", O_CREAT, 0666, 1);
    Sem_vending_act = sem_open("/Sem_vending_act", O_CREAT, 0666, 1);

    /*服务柜初始化*/
    GOODS_Init();


    // 创建管道
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        return 1;
    }    
    if (pipe(pipe_act_fd) == -1) {
        perror("pipe");
        return 1;
    }

    /*创建三个子进程*/
    Express_pid = fork();
    if(Express_pid == 0)
    {
        while(1)
        {
            sem_wait(Sem_express);
            // printf("get express sem\n");
            pthread_create(&Express_tid, NULL, (void *)express, NULL);
        }
    }
    Food_pid = fork();
    if(Food_pid == 0)
    {
        while(1)
        {
            sem_wait(Sem_food);
            // printf("get food sem\n");
            pthread_create(&Food_tid, NULL, (void *)food, NULL);
        }
    }

    Vending_pid = fork();
    if(Vending_pid == 0)
    {
        while(1)
        {
            sem_wait(Sem_vending);
            pthread_create(&Vending_tid, NULL, (void *)vending, NULL);
        }
    }

    /*服务器初始化*/
	lsfd = socket(PF_INET,SOCK_STREAM,0);	
	if(lsfd<0)
	{
		perror("socket() fail\n");
		return -1;
	}
	bzero(&my_addr,sizeof(struct sockaddr_in));
	my_addr.sin_family =  PF_INET;	
	my_addr.sin_port   =  htons(portnumber);
	my_addr.sin_addr.s_addr   =  htonl(INADDR_ANY);
	addrLen = sizeof(struct sockaddr_in);
	
	if(bind(lsfd,(struct sockaddr* )&my_addr ,addrLen)<0)
	{
		perror("bind() fail\n");
		return -1;		
	}
	
	listen(lsfd,5);
	cli_addr_len = sizeof(struct sockaddr_in);


	while(1)
	{
		newfd = accept(lsfd,(struct sockaddr *)&cli_adr, &cli_addr_len);
		
        printf("client:ip:%s   port:%d\n", inet_ntoa(cli_adr.sin_addr), cli_adr.sin_port);
				
		arg = malloc(sizeof(int));
		*arg = newfd;//必须搞清楚为什么要申请内存
		
		pthread_create(&Accept_tid, NULL, accept_thread_func, (void*)arg);	
	}


	close(newfd);
	close(lsfd);
}
