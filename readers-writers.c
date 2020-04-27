/*
    Gabriel Bonatto Justo

    compile:
    gcc -pthread readers-writers.c -o readers-writers
    
    run:
    ./readers-writers
*/


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>


int* flag;
void* writer();







int main (int argc, char* argv[])
{
    pthread_t wr;
    pthread_t re;
    flag  = malloc(2*sizeof(int));
    flag[0] = 0;
    flag[1] = 0;
    pthread_create(&wr, NULL, writer, NULL);

}



void* writer()
{
    printf("%d", flag[0]);
}