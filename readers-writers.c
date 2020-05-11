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
bool reading;
FILE * fp;

void* writer();
void* reader();
void lockWriter(int);
void lockReader(int);
void unlock(int);
bool everyOneBehind(int);
bool everyWriterBehind(int);






int main (int argc, char* argv[])
{
    
    fp = fopen("./text.txt", "w");
    reading = false;
    int err_code = 0;
    nReaders = atoi(argv[1]);
    nWriters = atoi(argv[2]);
    pthread_t wr[nWriters];
    pthread_t re[nReaders];
    level  = malloc((nWriters + nReaders) * sizeof(int));
    last_to_enter = malloc(((nWriters + nReaders)-1) * sizeof(int));
   // fprintf(fp, "Hello World!!!");
    for(int i=0; i<nWriters; i++)
    {
        
        err_code = pthread_create(&wr[i], NULL, writer, (void*) i);
        printf("criei o processo %d \n", i);
        if (err_code)
        {
			printf("ERROR code is %d\n", err_code);
			break;
		}
        //pthread_join(wr[i], NULL);
    }
    printf("sai do for \n");
    fclose(fp);
    
   pthread_exit(NULL);
}



void* writer(void* id)
{
    int my_id = (int)id;
    printf("my id is %d \n", id);
    lockWriter(my_id);
    
    reading = false;
    
    fprintf(fp, "processo %d escrevendo \n", id);
    unlock(id);
    printf("processo %d terminou \n", id);
}

void lockWriter (int id)
{
    for (int i = 0; i < nWriters + nReaders; i++)
    {
        level[id] = i;
        last_to_enter[i] = id;
        while (last_to_enter[i] == id && everyOneBehind(id)){ }
    }
    
}


void* reader(void* id)
{
    int my_id = (int)id;
    lockReader(id);
    reading = true;

    unlock(id);
    
}

void lockReader(int id)
{
    for (int i = 0; i < nWriters + nReaders; i++)
    {
        level[id] = i;
        last_to_enter[i] = id;
        while (last_to_enter[i] == id && everyWriterBehind(id) && reading == false){}
    }
    
}


void unlock (int id){level[id] = -1;}

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
        if(level[i] > level[id] && i != id)
            return true;
    }
    return false;
}