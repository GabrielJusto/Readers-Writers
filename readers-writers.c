/*
    Gabriel Bonatto Justo Gabriel Paiz

    compile:
    gcc -pthread readers-writers.c -o readers-writers
    
    run:
    ./readers-writers <readers number> <writers number>
*/


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>


int* level;
int* last_to_enter;
int* readers;
int nReaders;
int nWriters;
int reading;
FILE * fp;
FILE * fpr;

void* writer();
void* reader();
void lockWriter(int);
void lockReader(int);
void unlock(int);
bool everyOneBehind(int);
bool everyWriterBehind(int);
bool thereAreReaders();
void unlockReader(int id);






int main (int argc, char* argv[])
{
    FILE * freset = fopen("./text.txt", "w");
    fclose(freset);
    reading = 0;
    int err_code = 0;
    nReaders = atoi(argv[1]);
    nWriters = atoi(argv[2]);
    pthread_t wr[nWriters];
    pthread_t re[nReaders];
    level  = malloc((nWriters + nReaders) * sizeof(int));
    last_to_enter = malloc(((nWriters + nReaders)-1) * sizeof(int));
    readers = malloc(sizeof(int) * nReaders);
    for (int i = 0; i < nReaders; i++)
        readers[i] = -1;
    
    
    for(int i=0; i<nWriters + nReaders; i++)
    {
        if (i < nWriters)
        {
            err_code = pthread_create(&wr[i], NULL, writer, (void*) i);
            if (err_code)
            {
                printf("ERROR code is %d\n", err_code);
                break;
            }
        }
        else
        {
            err_code = pthread_create(&re[i-nWriters], NULL, reader, (void*) i);
            if (err_code)
            {
                printf("ERROR code is %d\n", err_code);
                break;
            }
        }
        
        
        
    }
    sleep(5);
    
   pthread_exit(NULL);
}



void* writer(void* id)
{
    FILE * fpw;
    int my_id = (int)id;

    
    //  while (1)
    for(int i=0; i<100; i++)
    {
        
        lockWriter(my_id);
        printf("Writer %d \n", my_id);
        //sleep(5);
        
        fpw = fopen("./text.txt", "a");
     
        fprintf(fpw, "processo %d escrevendo \n", my_id);
        fclose(fpw);

        unlock(my_id);

    } 
}

void lockWriter (int id)
{
    for (int i = 0; i < nWriters + nReaders; i++)
    {
        level[id] = i;
        last_to_enter[i] = id;
        while ((last_to_enter[i] == id && everyOneBehind(id)) || reading > 0){ }
    }
    
}

bool thereAreReaders()
{
    for (int i = 0; i < nReaders; i++)
    {
        if (readers[i] != -1)
            return true;
    }
    return false;
    
}
void* reader(void* id)
{
    int my_id = (int)id;
    
    FILE * fpr = fopen("./text.txt", "r");
    char c = fgetc(fpr);
    for (int i = 0; i < 100; i++)
    {
        lockReader(my_id);
        reading ++;
        printf("Reader %d \n", my_id);
        //sleep(5);
        
        
        
        // printf("\n leitor %d: \n", my_id);
        // while (c != EOF) 
        // { 
        //     printf ("%c", c); 
        //     c = fgetc(fpr); 
        // } 
        fclose(fpr);
        reading --;
        unlock(my_id);
        
    }
    
    
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
    for (int i = 0; i < nWriters + nReaders; i++) 
        if(level[i] > level[id])
            return true;
    return false;
}
bool everyOneBehind(int id)
{
    for (int i = 0; i < nWriters + nReaders; i++)
    {
        if(level[i] > level[id] && i != id)
            return true;
    }
    return false;
}