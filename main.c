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

//attribute used for defining scheduling policy to FIFO
pthread_attr_t attribute;

//Counter of how many cars are in the city
volatile int miastoA_ilosc=0;
volatile int miastoB_ilosc=0;

//Counter of how many cars are trying to get through the bridge
volatile int miastoA_wyjazd=0;
volatile int miastoB_wyjazd=0;

/** \brief Return randomize number
 * \return random number
 */
unsigned ll llrand()
{
    unsigned ll a = rand()*rand(); if (a<0) a*-1; 
    return a;
}

/** \brief Return randomize number from given range
 *
 * \param a start of range to pick number
 * \param b end of range to pick number
 * \return random number
 *
 */
ll rn(ll a,ll b)
{
    return ((unsigned ll)(llrand()))%(b-a+1)+a;
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
        pthread_mutex_lock(&lock);
        if(samochod->miasto=='A' && miastoA_wyjazd>0)
        {
            samochod->miasto='B';
            miastoA_wyjazd--;
            printf("A-%d %d>>> [>> %d >>] <<<%d %d-B\n",miastoA_ilosc,miastoA_wyjazd,samochod->nr,miastoB_wyjazd,miastoB_ilosc);
            fflush(stdout);
            miastoB_ilosc++;
        }
        else if(samochod->miasto=='B' && miastoB_wyjazd>0)
        {
            samochod->miasto='A';
            miastoB_wyjazd--;
            printf("A-%d %d>>> [<< %d <<] <<<%d %d-B\n",miastoA_ilosc,miastoA_wyjazd,samochod->nr,miastoB_wyjazd,miastoB_ilosc);
            fflush(stdout);
            miastoA_ilosc++;
        }
        pthread_mutex_unlock(&lock);
        pthread_mutex_unlock(&lock2);
    }
}

int main(int argc, char** argv)
{
    if(argc!=2) 
    {
        printf("Zla liczba argumentow");
        return EXIT_FAILURE;
    }
    time_t t;
    srand((unsigned) time(&t));
    int ile_watkow = atoi(argv[1]);
    int i;
    Samochod** samochody = (Samochod**)malloc(ile_watkow*sizeof(Samochod*));
    pthread_t* tid = (pthread_t*)malloc(ile_watkow*sizeof(pthread_t));
    pthread_mutex_init(&lock,NULL);
    pthread_mutex_init(&lock2,NULL);
    pthread_attr_init(&attribute);
    pthread_attr_setschedpolicy(&attribute,SCHED_FIFO);
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

    for(i=0;i<ile_watkow;i++)
    {
        pthread_create(&tid[i],&attribute,Przejazd,samochody[i]);
    }
    for(i=0;i<ile_watkow;i++)
    {
        pthread_join(tid[i],NULL);
    }
    //pthread_exit(NULL);

    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&lock2);

    for(i=0;i<ile_watkow;i++)
    {
        free(samochody[i]);
    }
    free(samochody);
    return 0;
}