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
#include <cmath>
#include <time.h>


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
int map_commodity(string commodity)
{
    std::string commodities[NUM_COMM] = {"ALUMINIUM","COPPER","COTTON",
                                         "CRUDEOIL","GOLD","LEAD","MENTHAOIL","NATURALGAS","NICKEL","SILVER","ZINC"
                                        };
    for(int i=0; i<NUM_COMM; i++)
    {
        if((strcasecmp(commodity.c_str(),commodities[i].c_str()))==0)
        {
            return i;
        }
    }
    return -1;
}
void print_current_time(){
time_t current_time ;
    struct tm *now;
    struct timespec nano;
    char buff[80];

        time(&current_time);
        now = localtime(&current_time);
        clock_gettime(CLOCK_REALTIME, &nano);
        strftime(buff,80,"%m/%d/%Y %H:%M:%S",now);
        fprintf(stderr,"[%s.%3d]",buff,(int)nano.tv_nsec);

}
const key_t mutex_key = 444;
const   key_t buffer_key = 555;
const   key_t empty_key = 666;
int main(int argc, char** argv)
{
    if(argc != 5)
    {
        printf("incorrect arguments!\n");
        return 0;
    }
    int commodity_index =map_commodity(argv[1]);
    if(commodity_index == -1)
    {
        printf("commodity doesn't exist!\n");
        return 0;
    }
    int sleep_time = (int)(std::stoi(argv[4])/1000);
    double mean = std::stod(argv[2]);
    double std_dev = std::stod(argv[3]);
    //get size of buffer
    key_t key_buffer_in = 111;
    //ftok("buffer_size_",65);
    int size_buffer_in = sizeof(int)*2;
    int shmid1 = shmget(key_buffer_in,size_buffer_in,0666|IPC_CREAT);
    int *in_index = (int *) shmat(shmid1,NULL,0);
    int in = in_index[0];
    int buffer_size = in_index[1];

    std::default_random_engine generator;
    int memory_size = sizeof(commodity_price)* buffer_size;
    key_t key2 = 222;
    //ftok("comm_memory",65);
    int shmid2 = shmget(key2,memory_size,0666|IPC_CREAT);
    commodity_price *commodities_prices = (commodity_price *) shmat(shmid2,NULL,0);
    double price;


    int mutex_semaphore,buffer_semaphore, empty_semaphore;

    if ((mutex_semaphore = semget(mutex_key,1,0))==-1)
    {
        perror("semget mutex");
        exit(1);
    }

    if ((buffer_semaphore = semget(buffer_key,1,0))==-1)
    {
        perror("semget buffer");
        exit(1);
    }
    if ((empty_semaphore = semget(empty_key,1,0))==-1)
    {
        perror("semget empty");
        exit(1);
    }
    struct sembuf sem_lock = { 0, -1,0 };

    struct sembuf sem_unlock = {0, 1, 0 };

    std::normal_distribution<double> distribution(mean,std_dev);
    while(true)
    {
        price = abs(distribution(generator));
    print_current_time();
        fprintf(stderr," %s : generating a new value %7.2lf\n",argv[1],price);

        //start critical section
        if (semop (empty_semaphore, &sem_lock, 1) == -1)
        {
            perror ("semop: buffer semaphore");
            exit (1);
        }
       print_current_time();
        fprintf(stderr," %s : trying to get mutex on shared buffer\n",argv[1]);

        if (semop (mutex_semaphore, &sem_lock, 1) == -1)
        {
            perror ("semop: mutex semaphore");
            exit (1);
        }
        print_current_time();
        fprintf(stderr," %s : placing %7.2lf on shared buffer\n",argv[1],price);

        in= in_index[0];

            in = (in+1)%buffer_size;

        commodities_prices[in].comm_index = commodity_index;
        commodities_prices[in].price = price;
        in_index[0]=in;
        if (semop (mutex_semaphore, &sem_unlock, 1) == -1)
        {
            perror ("semop: mutex_sem");
            exit (1);
        }
        if (semop (buffer_semaphore, &sem_unlock, 1) == -1)
        {
            perror ("semop: empty_sem");
            exit (1);
        }
        print_current_time();
        fprintf(stderr," %s : sleeping for %d ms\n",argv[1],std::stoi(argv[4]));

        sleep(sleep_time);

    }
    shmdt(commodities_prices);
    return 0;
}

