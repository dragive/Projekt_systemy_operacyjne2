/**
 * @file main_cond.c
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
    int kolejka;
}Samochod;

//mutexes used for blocking 
pthread_mutex_t lock;
pthread_mutex_t lock2;

//attribute used for defining scheduling policy to FIFO
pthread_attr_t attribute;


pthread_cond_t condition= PTHREAD_COND_INITIALIZER;


//how many
volatile int miastoA_ilosc=0;
volatile int miastoB_ilosc=0;

//ile samochodow czeka na przejazd
volatile int miastoA_wyjazd=0;
volatile int miastoB_wyjazd=0;

unsigned ll llrand()
{
    unsigned ll a = rand()*rand(); if (a<0) a*-1; 
    return a;
}
ll rn(ll a,ll b)
{
    return ((unsigned ll)(llrand()))%(b-a+1)+a;
}


volatile int jedzie =0;

//funkcja watka
void* Przejazd(void* vargp)
{
    sleep(1);
    Samochod* samochod = (Samochod*)vargp;
    int i;
    int temp;
    int debug=0;
    
    while(1)
    {
        temp = 1;
        usleep(rn(100,10000));
        pthread_mutex_lock(&lock);
        if(debug)printf("%d",temp++);fflush(stdout);//1
        if(samochod->miasto=='A' && miastoA_ilosc>0)
        {
            miastoA_ilosc--;
            miastoA_wyjazd++;
        }
        else if(samochod->miasto=='B' && miastoB_ilosc>0)
        {
            miastoB_ilosc--;
            miastoB_wyjazd++;
        }
       if(debug) printf("%d",temp++);fflush(stdout);//2
        usleep(rn(100,10000));

        pthread_mutex_unlock(&lock);     

        if(debug)printf("%d",temp++);fflush(stdout);//3
        usleep(rn(1000,100000));
        
        
        pthread_mutex_lock(&lock);
        if(debug)printf("%d",temp++);fflush(stdout);//4
        samochod->kolejka=1;
        while(jedzie){
            pthread_cond_wait(&condition,&lock);
        }
        if(debug)printf("%d",temp++);fflush(stdout); //5
        samochod->kolejka=0;
        usleep(rn(1000,2000));
        if(debug)printf("%d",temp++);fflush(stdout);//6
        jedzie =1;
        
        if(samochod->miasto=='A' && miastoA_wyjazd>0)
        {
            samochod->miasto='B';
            miastoA_wyjazd--;
            printf("A-%d %d>>> [>> %3.d >>] <<<%d %d-B\n",miastoA_ilosc,miastoA_wyjazd,samochod->nr,miastoB_wyjazd,miastoB_ilosc);
            fflush(stdout);
            miastoB_ilosc++;
        }
        else if(samochod->miasto=='B' && miastoB_wyjazd>0)
        {
            samochod->miasto='A';
            miastoB_wyjazd--;
            printf("A-%d %d>>> [<< %3.d <<] <<<%d %d-B\n",miastoA_ilosc,miastoA_wyjazd,samochod->nr,miastoB_wyjazd,miastoB_ilosc);
            fflush(stdout);
            miastoA_ilosc++;
        }
        if(debug)printf("%d",temp++);fflush(stdout);//7
        usleep(rn(1000,100000));
        jedzie =0;
        pthread_cond_signal(&condition);
        if(debug)printf("%d",temp++);fflush(stdout);//8
        pthread_mutex_unlock(&lock);
        if(debug)printf("%d",temp++);fflush(stdout);//9
       
        // printf("#%d",samochod->nr);
        // // pthread_cond_signal(&condition);//-1+(samochod->nr)+
        






    }
}

//funkcja watka
void* PrzejdzDoKolejki(void* vargp)
{
   
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
    condition ;//= malloc(ile_watkow*sizeof(pthread_cond_t));
    int i=0;
    // for(;i<ile_watkow;i++)
    // {
    //     pthread_cond_init(&condition[i],NULL);
    // }
    
    Samochod** samochody = (Samochod**)malloc(ile_watkow*sizeof(Samochod*));
    pthread_t* tid = (pthread_t*)malloc(2*ile_watkow*sizeof(pthread_t));
    pthread_mutex_init(&lock,NULL);
    pthread_mutex_init(&lock2,NULL);
    pthread_attr_init(&attribute);
    pthread_attr_setschedpolicy(&attribute,SCHED_FIFO);
    for(i=0;i<ile_watkow;i++)
    {
        samochody[i] = (Samochod*)malloc(sizeof(Samochod));
        samochody[i]->nr=i+1;
        samochody[i]->kolejka=0;
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
        pthread_create(&tid[i],&attribute,PrzejdzDoKolejki,samochody[i]);
        pthread_create(&tid[ile_watkow+i],&attribute,Przejazd,samochody[i]);
    }
    for(i=0;i<ile_watkow*2;i++)
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
    //free(condition);
    return 0;
}