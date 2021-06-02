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

//ile jest w miescie
int miastoA_ilosc=0;
int miastoB_ilosc=0;

//ile samochodow czeka na przejazd
int miastoA_wyjazd=0;
int miastoB_wyjazd=0;

//funkcja watka
void* Przejazd(void* vargp)
{
    Samochod* samochod = (Samochod*)vargp;
    printf("%d ",samochod->nr);

    if(samochod->miasto=='A')
    {
        printf("A-%d %d>>> [>> %d >>] <<<%d %d-B",miastoA_ilosc,--miastoA_wyjazd,samochod->nr,miastoB_wyjazd,miastoB_ilosc++);
        samochod->miasto='B';
    }
    else
    {
        printf("A-%d %d>>> [<< %d <<] <<<%d %d-B",miastoA_ilosc++,miastoA_wyjazd,samochod->nr,--miastoB_wyjazd,miastoB_ilosc);
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

    for(i=0;i<ile_watkow;i++)
    {
        pthread_create(&tid[i],NULL,Przejazd,samochody[i]);
    }

    printf("%d %d",miastoA_wyjazd,miastoB_wyjazd);

    for(i=0;i<ile_watkow;i++)
    {
        free(samochody[i]);
    }
    free(samochody);
    pthread_exit(NULL);
    return 0;
}