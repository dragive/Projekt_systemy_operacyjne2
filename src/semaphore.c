#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include<pthread.h>

sem_t semafor;

void first(){
    int n =10;
    while(n-->=0){
        printf("waiting1");fflush(stdout);
        sem_wait(&semafor);
        printf("sleep1");fflush(stdout);
        sleep(1);
        printf("post1");fflush(stdout);
        sem_post(&semafor);
        printf("sem post1");fflush(stdout);
    }

}

void second(){
    int n =3;
    while(n-->=0){
        printf("waiting2");fflush(stdout);

        sem_wait(&semafor);
        printf("sleep2");fflush(stdout);
        sleep(1);
        printf("post2");fflush(stdout);
        sem_post(&semafor);
        printf("sem post2");fflush(stdout);
    }
}

int main(){

    sem_init(&semafor,0,1);

    pthread_t th1,th2;
    pthread_create(&th1,NULL,&(first),NULL);
    pthread_create(&th2,NULL,&(second),NULL);

    sem_destroy(&semafor);
    pthread_exit(NULL);
}
