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
    Samochod* samochod = (Samochod*)vargp;
    if(samochod->miasto=='A')
    {
        //printf("A-%d %d>>> [>> %d >>] <<<%d %d-B\n",miastoA_ilosc,--miastoA_wyjazd,samochod->nr,miastoB_wyjazd,miastoB_ilosc++);
        printf("A-%d %d>>> [>> %d >>] <<<%d %d-B\n",miastoA_ilosc,--miastoA_wyjazd,counter++,miastoB_wyjazd,miastoB_ilosc++);
        samochod->miasto='B';
    }
    else
    {
        printf("A-%d %d>>> [<< %d <<] <<<%d %d-B\n",miastoA_ilosc++,miastoA_wyjazd,samochod->nr,--miastoB_wyjazd,miastoB_ilosc);
        printf("A-%d %d>>> [<< %d <<] <<<%d %d-B\n",miastoA_ilosc++,miastoA_wyjazd,counter++,--miastoB_wyjazd,miastoB_ilosc);
        samochod->miasto='A';
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
        pthread_create(&tid[i],NULL,Przejazd,samochody[i]);
    }
    for(i=0;i<ile_watkow;i++)
    {
        pthread_join(tid[i],NULL);
    }
    //pthread_exit(NULL);

    for(i=0;i<ile_watkow;i++)
    {
        free(samochody[i]);
    }
    free(samochody);
    return 0;
}