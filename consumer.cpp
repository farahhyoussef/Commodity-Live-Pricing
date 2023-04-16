#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <cstring>
#include <math.h>
#include <random>
//#include <queue>
#include<sys/ipc.h>
#include<sys/shm.h>
#include "queue.h"
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#define NUM_COMM 11
using namespace std;
typedef struct
{
    int comm_index;
    double price;
} commodity_price;

union semun
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};
string map_commodity(int commodity_index)
{
    std::string commodities[NUM_COMM] = {"ALUMINIUM","COPPER","COTTON",
                                         "CRUDEOIL","GOLD","LEAD","MENTHAOIL","NATURALGAS","NICKEL","SILVER","ZINC"
                                        };
    return commodities[commodity_index];
}
void init_mem(int buffer_size)
{

}
const key_t mutex_key = 444;
const   key_t buffer_key = 555;
const   key_t empty_key = 666;
void init_semaphores(int buffer_size)
{



}



void print_prices()
{
    printf("\e[1;1H\e[2J\n");
    printf("+-------------------------------------+\n");
    printf("| Currency      |  Price   | AvgPrice |\n");
    printf("+-------------------------------------+\n");
    printf("| ALUMINIUM     |    \033[;34m0.00\033[0m  |    \033[;34m0.00\033[0m  |\n");
    printf("| Copper        |    \033[;34m0.00\033[0m  |    \033[;34m0.00\033[0m  |\n");
    printf("| COTTON        |    \033[;34m0.00\033[0m  |    \033[;34m0.00\033[0m  |\n");
    printf("| CRUDEOIL      |    \033[;34m0.00\033[0m  |    \033[;34m0.00\033[0m  |\n");
    printf("| GOLD          |    \033[;34m0.00\033[0m  |    \033[;34m0.00\033[0m  |\n");
    printf("| LEAD          |    \033[;34m0.00\033[0m  |    \033[;34m0.00\033[0m  |\n");
    printf("| MENTHAOIL     |    \033[;34m0.00\033[0m  |    \033[;34m0.00\033[0m  |\n");
    printf("| NATURALGAS    |    \033[;34m0.00\033[0m  |    \033[;34m0.00\033[0m  |\n");
    printf("| NICKEL        |    \033[;34m0.00\033[0m  |    \033[;34m0.00\033[0m  |\n");
    printf("| SILVER        |    \033[;34m0.00\033[0m  |    \033[;34m0.00\033[0m  |\n");
    printf("| ZINC          |    \033[;34m0.00\033[0m  |    \033[;34m0.00\033[0m  |\n");
    printf("+-------------------------------------+\n");
    printf("\033[18;1H\n");
}

void update_print(int comm_index, double prev_price,double current_price, double avg_price,double prev_avg)
{
    int printing_index = comm_index+5;
    //printf("%f",current_price);
    printf("\033[1;1H\n");
    if(current_price>prev_price)
    {
        printf("\033[%d;18H \033[;32m%7.2lf↑\033[0m ",printing_index,current_price);
    }
    else
    {
        printf("\033[%d;18H \033[;31m%7.2lf↓\033[0m ",printing_index,current_price);
    }
    printf("\033[1;1H\n");
    if(avg_price>prev_avg)
    {
        printf("\033[%d;29H \033[;32m%7.2lf↑\033[0m ",printing_index,avg_price);
    }
    else
    {
        printf("\033[%d;29H \033[;31m%7.2lf↓\033[0m ",printing_index,avg_price);
    }
    printf("\033[18;1H\n");

}
int main(int argc, char** argv)
{
    if(argc != 2)
    {
        printf("incorrect arguments!\n");
        return 0;
    }

    int buffer_size = std::stoi(argv[1]);
    //send buffer size to producer
    key_t key_buffer_in = 111;
    //ftok("buffer_size_",65);
    int size_buffer_in = sizeof(int)*2;
    int shmid1 = shmget(key_buffer_in,size_buffer_in,0666|IPC_CREAT);
    int *in_buffer = (int *) shmat(shmid1,NULL,0);
    in_buffer[0]=-1;
    in_buffer[1] = buffer_size;



    //shared memmory
    int memory_size = sizeof(commodity_price)* buffer_size;
    key_t key2 = 222;
    //ftok("comm_memory",65);
    int shmid2 = shmget(key2,memory_size,0666|IPC_CREAT);
    if(shmid2==-1){
    perror("shared memory");}
    commodity_price *commodities_prices = (commodity_price *) shmat(shmid2,NULL,0);

    double prev_price=0.0;
    double prev_average=0.0;

    union semun arg;


    int mutex_semaphore,buffer_semaphore, empty_semaphore;
    if ((mutex_semaphore = semget(mutex_key,1,0666 | IPC_CREAT))==-1)
    {
        perror("semget mutex");
        exit(1);
    }
    arg.val = 1;
    if (semctl (mutex_semaphore, 0, SETVAL, arg) == -1)
    {
        perror ("semctl SETVAL mutex");
        exit (1);
    }
    if ((buffer_semaphore = semget(buffer_key,1,0666 | IPC_CREAT))==-1)
    {
        perror("semget buffer");
        exit(1);
    }
    arg.val = 0;
    if (semctl (buffer_semaphore, 0, SETVAL, arg) == -1)
    {
        perror ("semctl SETVAL buffer");
        exit (1);
    }
    if ((empty_semaphore = semget(empty_key,1,0666 | IPC_CREAT))==-1)
    {
        perror("semget empty");
        exit(1);
    }
    arg.val = buffer_size;
    if (semctl (empty_semaphore, 0, SETVAL, arg) == -1)
    {
        perror ("semctl SETVAL empty");
        exit (1);
    }

    print_prices();
    Queue prices_track[NUM_COMM];
    int commodity_index;
    double current_price,average_price;
    struct sembuf sem_lock = { 0, -1,0 };

    struct sembuf sem_unlock = {0, 1, 0 };
    int out = -1;
    while(true)
    {
        //start critical section
        //buffer not empty
        if (semop (buffer_semaphore, &sem_lock, 1) == -1)
        {
            perror ("semop: buffer semaphore");
            exit (1);
        }
        if (semop (mutex_semaphore, &sem_lock, 1) == -1)
        {
            perror ("semop: mutex semaphore");
            exit (1);
        }


        out = (out+1)%buffer_size;
        commodity_index = commodities_prices[out].comm_index ;
        current_price = commodities_prices[out].price ;

        //end critical section
        if (semop (mutex_semaphore, &sem_unlock, 1) == -1)
        {
            perror ("semop: mutex_sem");
            exit (1);
        }
        //increase empty slots by one
        if (semop (empty_semaphore, &sem_unlock, 1) == -1)
        {
            perror ("semop: empty_sem");
            exit (1);
        }
        prices_track[commodity_index].insert_queue(current_price);
        average_price = prices_track[commodity_index].average();
        update_print(commodity_index,  prev_price, current_price, average_price,prev_average);
        prev_price = current_price;
        prev_average = average_price;

    }
    shmdt(commodities_prices);
    printf("\033[15;0H");
    return 0;
}

