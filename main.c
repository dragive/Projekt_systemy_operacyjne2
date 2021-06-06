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

typedef struct Samochod
{
    int nr;
    char miasto;
}Samochod;

pthread_mutex_t lock;
pthread_attr_t attribute;

//counter
volatile int counter=0;

//ile jest w miescie
volatile int miastoA_ilosc=0;
volatile int miastoB_ilosc=0;

//ile samochodow czeka na przejazd
volatile int miastoA_wyjazd=0;
volatile int miastoB_wyjazd=0;

//funkcja watka
void* Przejazd(void* vargp)
{
    pthread_mutex_lock(&lock);
    Samochod* samochod = (Samochod*)vargp;
    int i;

    //licznik
    counter++;

    //to do czekania bylo zeby zwizulizowac jak zachowa sie zwiekszanie countera bez mutexow z nizsza petla(efekt byl taki ze od razu counter polecial do maxa przy wypisywaniu)
    for(i=0; i<__INT_MAX__;i++);

    if(samochod->miasto=='A')
    {
        //printf("A-%d %d>>> [>> %d >>] <<<%d %d-B\n",miastoA_ilosc,--miastoA_wyjazd,samochod->nr,miastoB_wyjazd,miastoB_ilosc++);
        printf("A-%d %d>>> [>> %d >>] <<<%d %d-B SAMOCHOD:%d\n",miastoA_ilosc,--miastoA_wyjazd,counter,miastoB_wyjazd,miastoB_ilosc++,samochod->nr);
        samochod->miasto='B';
    }
    else
    {
        //printf("A-%d %d>>> [<< %d <<] <<<%d %d-B\n",miastoA_ilosc++,miastoA_wyjazd,samochod->nr,--miastoB_wyjazd,miastoB_ilosc);
        printf("A-%d %d>>> [<< %d <<] <<<%d %d-B SAMOCHOD:%d\n",miastoA_ilosc++,miastoA_wyjazd,counter,--miastoB_wyjazd,miastoB_ilosc,samochod->nr);
        samochod->miasto='A';
    }
    pthread_mutex_unlock(&lock);
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

    for(i=0;i<ile_watkow;i++)
    {
        free(samochody[i]);
    }
    free(samochody);
    return 0;
}