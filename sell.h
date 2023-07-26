#ifndef __SELL_H_
#define __SELL_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/msg.h>

#define MAX_GOODS_NUM 10

typedef enum{
    REQUEST_NONE,
    REQUEST_PUT_EXPRESS,
    REQUEST_PUT_FOOD,
    REQUEST_GET_EXPRESS,
    REQUEST_GET_FOOD,
    REQUEST_VENDING
}RequestType;

typedef enum{
    GOODS_EXPRESS,
    GOODS_FOOD,
    GOODS_VENDING
}GoodsType;


typedef struct{
    RequestType type;
    int         userID;
    int         Num;
}RequestPack;


typedef struct{
    GoodsType type;
    int       status;
    int       num;
}GoodsInfo;

#endif