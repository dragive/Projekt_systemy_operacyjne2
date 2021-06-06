/**
 * @file main.c
 * @author Kacper Chrost, Maciej Fender, Krzysztof Funkowski
 * @brief Solved bridge problem using mutexes and condition variables
 * @version 1.0
 * @date 2021-05-06
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#define ll long long

/** \brief Structure representing a car having information about it's number and city where it actually is. 
 */
typedef struct Samochod
{
    int nr;
    char miasto;
}Samochod;

//mutexes used for blocking 
pthread_mutex_t lock;
pthread_mutex_t lock2;
pthread_mutex_t lock_rn;
//attribute used for defining scheduling policy to FIFO
pthread_attr_t attribute;

//Counter of how many cars are in the city
volatile int miastoA_ilosc=0;
volatile int miastoB_ilosc=0;

//Counter of how many cars are trying to get through the bridge
volatile int miastoA_wyjazd=0;
volatile int miastoB_wyjazd=0;

/** \brief Return randomized number
 * \return random number
 */
unsigned ll llrand()
{
    unsigned ll a = rand()*rand(); if (a<0) a*-1; 
    return a;
}
/** \brief Return randomized number from given range
 *
 * \param a start of range to pick number
 * \param b end of range to pick number
 * \return random number
 *
 */
ll rn(ll a,ll b)
{
    pthread_mutex_lock(&lock_rn);
    unsigned long long ret = ((unsigned ll)(llrand()))%(b-a+1)+a;
    pthread_mutex_unlock(&lock_rn);
    return ret;
}

/** \brief Thread function representiong car that want to go through bridge
 *
 * \param vargp a car entity
 *
 */
void* Przejazd(void* vargp)
{
    //sleep to wait for all threads to start working
    sleep(1);

    //casting to Samochod struct that represents car entity
    Samochod* samochod = (Samochod*)vargp;
    int i;

    //loop that represents cycle of getting from one city to another one
    while(1)
    {
        //randomized sleeping time for each thread
        usleep(rn(1000,1000000));

        //block that represents situation that car wants get back from the city and wants to queue up under a bridge
        pthread_mutex_lock(&lock);

        //if car is from City A and counter of cars in this city is greater than 0
        if(samochod->miasto=='A' && miastoA_ilosc>0)
        {
            //subract one from counter of cars in city A and add this one to queue on the side of city A
            miastoA_ilosc--;
            miastoA_wyjazd++;
        }
        //if car is from City B and counter of cars in this city is greater than 0
        else if(samochod->miasto=='B' && miastoB_ilosc>0)
        {
            //subract one from counter of cars in city B and add this one to queue on the side of city B
            miastoB_ilosc--;
            miastoB_wyjazd++;
        }
        pthread_mutex_unlock(&lock);

        //randomized sleeping time for each thread
        usleep(rn(1000,1000000));

        //block that represents situation that a car drives over the bridge
        pthread_mutex_lock(&lock2);
        //blocking first lock to prevent queueing up cars while one of them is on the bridge
        pthread_mutex_lock(&lock);

        //if car is from City A and counter of cars on this side of the bridge than 0
        if(samochod->miasto=='A' && miastoA_wyjazd>0)
        {
            //changing city where the car is driving
            samochod->miasto='B';
            //subract one from counter of cars in queue of city A side of the bridge
            miastoA_wyjazd--;
            //print current state of bridge and amount of cars in cities and queues
            printf("A-%d %d>>> [>> %d >>] <<<%d %d-B\n",miastoA_ilosc,miastoA_wyjazd,samochod->nr,miastoB_wyjazd,miastoB_ilosc);
            fflush(stdout);
            //add one to counter of cars in city B
            miastoB_ilosc++;
        }
        //if car is from City B and counter of cars on this side of the bridge than 0
        else if(samochod->miasto=='B' && miastoB_wyjazd>0)
        {
            //changing city where the car is driving
            samochod->miasto='A';
            //subract one from counter of cars in queue of city B side of the bridge
            miastoB_wyjazd--;
            //print current state of bridge and amount of cars in cities and queues
            printf("A-%d %d>>> [<< %d <<] <<<%d %d-B\n",miastoA_ilosc,miastoA_wyjazd,samochod->nr,miastoB_wyjazd,miastoB_ilosc);
            fflush(stdout);
            //add one to counter of cars in city A
            miastoA_ilosc++;
        }
        //unlocking mutexes
        pthread_mutex_unlock(&lock);
        pthread_mutex_unlock(&lock2);
    }
}

int main(int argc, char** argv)
{
    //if the amount of given parameteres is not equal 2 then end program
    if(argc!=2) 
    {
        printf("Zla liczba argumentow");
        return EXIT_FAILURE;
    }

    //implementing randomizing numbers depending on system time
    time_t t;
    srand((unsigned) time(&t));

    //set numbers of cars to variable from parameter
    int ile_watkow = atoi(argv[1]);
    int i;

    //initializing car entities and threads 
    Samochod** samochody = (Samochod**)malloc(ile_watkow*sizeof(Samochod*));
    pthread_t* tid = (pthread_t*)malloc(ile_watkow*sizeof(pthread_t));

    //initialize mutexes
    pthread_mutex_init(&lock,NULL);
    pthread_mutex_init(&lock2,NULL);
    pthread_mutex_init(&lock_rn,NULL);
    //initialize attribute
    pthread_attr_init(&attribute);
    //set scheduling policy to FIFO
    pthread_attr_setschedpolicy(&attribute,SCHED_FIFO);

    //initialize default settings for each car
    for(i=0;i<ile_watkow;i++)
    {
        samochody[i] = (Samochod*)malloc(sizeof(Samochod));
        samochody[i]->nr=i+1;
        if(rand()%2)
        {
            samochody[i]->miasto='A';
            miastoA_wyjazd++;
        }
        else
        {
            samochody[i]->miasto='B';
            miastoB_wyjazd++;
        }
    }

    printf("START MiastoA:%d MiastoB:%d\n",miastoA_wyjazd,miastoB_wyjazd);

    //start threads
    for(i=0;i<ile_watkow;i++)
    {
        pthread_create(&tid[i],&attribute,Przejazd,samochody[i]);
    }
    //wait for threads termination
    for(i=0;i<ile_watkow;i++)
    {
        pthread_join(tid[i],NULL);
    }
    //destroy mutexes
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&lock2);
    pthread_mutex_destroy(&lock_rn);
    //free allocated space
    for(i=0;i<ile_watkow;i++)
    {
        free(samochody[i]);
    }
    free(samochody);
    return 0;
}