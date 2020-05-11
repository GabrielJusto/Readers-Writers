/*
    Gabriel Bonatto Justo

    compile:
    gcc -pthread readers-writers.c -o readers-writers
    
    run:
    ./readers-writers <readers number> <writers number>
*/


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>


int* level;
int* last_to_enter;
int nReaders;
int nWriters;

void* writer();
void* reader();
void lockWriter(int);
void lockReader(int);
void unlock(int);
bool everyOneBehind(int);
bool everyWriterBehind(int);






int main (int argc, char* argv[])
{
    int err_code = 0;
    nReaders = atoi(argv[1]);
    nWriters = atoi(argv[2]);
    pthread_t wr;
    pthread_t re;
    level  = malloc((nWriters + nReaders) * sizeof(int));
    last_to_enter = malloc(((nWriters + nReaders)-1) * sizeof(int));

    for(int i=0; i<nWriters; i++)
    {
        err_code = pthread_create(&wr, NULL, writer, (void*) i);
        pthread_join(wr, NULL);
    }

    
   pthread_exit(NULL);
}



void* writer(void* id)
{
    int my_id = (int)id;
    lockWriter(my_id);
    //
    unlock(id);
    
    printf("thread %d say hello \n", my_id);
}

void lockWriter (int id)
{
    for (int i = 0; i < nWriters + nReaders; i++)
    {
        level[id] = i;
        last_to_enter[i] = id;
        while (last_to_enter[i] == id && everyOneBehind(id)){}
        
    }
    
}


void* reader(void* id)
{
    int my_id = (int)id;
    lockReader(id);

    unlock(id);
}

void lockReader(int id)
{
    for (int i = 0; i < nWriters + nReaders; i++)
    {
        level[id] = i;
        last_to_enter[i] = id;
        while (last_to_enter[i] == id && everyWriterBehind(id)){}
        
    }
    
}


void unlock (int id) { level[id] = -1; }

bool everyWriterBehind(int id)
{
    for (int i = 0; i < nWriters; i++) // só é testado se há escritores na frente pois não tem probleba dois leitores acessarem ao mesmo tempo
        if(level[i] > level[id])
            return false;
    return true;
}
bool everyOneBehind(int id)
{
    for (int i = 0; i < nWriters; i++)
    {
        if(level[i] > level[id])
            return false;
    }
    return true;
}