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

typedef struct Samochod
{
    int nr;
    char miasto;
}Samochod;

pthread_mutex_t lock;
pthread_mutex_t lock2;
pthread_attr_t attribute;
pthread_cond_t* condition;//= PTHREAD_COND_INITIALIZER;

//counter
volatile int counter=0;

//ile jest w miescie
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




//funkcja watka
void* Przejazd(void* vargp)
{
    sleep(1);
    Samochod* samochod = (Samochod*)vargp;
    int i;
    while(1)
    {
        usleep(rn(1000,1000000));

        pthread_mutex_lock(&lock);
        pthread_cond_wait(-1+(samochod->nr)+condition,&lock);
        printf("###%d\n",samochod->nr);
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
    }
}

//funkcja watka
void* PrzejdzDoKolejki(void* vargp)
{
    sleep(1);
    Samochod* samochod = (Samochod*)vargp;
    int i;
    while(1)
    {
        usleep(rn(1000,1000000));
        //usleep(1000000+rand()%1000000);
        pthread_mutex_lock(&lock);
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
        printf("#%d",samochod->nr);
        pthread_cond_signal(-1+(samochod->nr)+condition);
        pthread_mutex_unlock(&lock);
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
    condition = malloc(ile_watkow*sizeof(pthread_cond_t));
    int i=0;
    for(;i<ile_watkow;i++)
    {
        pthread_cond_init(&condition[i],NULL);
    }
    
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
    free(condition);
    return 0;
}