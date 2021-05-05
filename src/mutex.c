#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

#include <fcntl.h>
#include<pthread.h>

pthread_mutex_t mutex1;


void second(){
    int n =1;
    while(n-->=0){
        printf("waiting2 for unlock\n");fflush(stdout);
        pthread_mutex_lock(&mutex1);

        printf("sleep2\n");fflush(stdout);
        sleep(1);
        printf("post2\n");fflush(stdout);
        pthread_mutex_unlock(&mutex1);
        printf("sem post2\n ");fflush(stdout);
    }
}

void first(){
    int n =1;
    while(n-->=0){
        printf("waiting for unlock\n");fflush(stdout);
        pthread_mutex_lock(&mutex1);

        printf("sleep1\n");fflush(stdout);
        sleep(1);
        printf("post1\n");fflush(stdout);
        pthread_mutex_unlock(&mutex1);
        printf("sem post1\n");fflush(stdout);
    }

}

int main(){

    pthread_mutex_init(&mutex1, NULL);
    pthread_t th1,th2;
    pthread_create(&th1,NULL,&(first),NULL);
    pthread_create(&th2,NULL,&(second),NULL);
    printf("1111");

    //pthread_join(&th1,NULL);
    printf("2222");


    printf("1111");
    pthread_mutex_destroy(&mutex1);
    pthread_exit(NULL);


}
