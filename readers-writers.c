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
#include <unistd.h>


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
    fclose(fp);
    
   pthread_exit(NULL);
}



void* writer(void* id)
{

    int my_id = (int)id;
    // while (1)
    for(int i=0; i<100; i++)
    {
        lockWriter(my_id);
        reading = false;   
        fprintf(fp, "processo %d escrevendo \n", id);
        unlock(id);
    } 
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
    char buff[255];

    fscanf(fp, "%s", buff);
    printf("1 : %s\n", buff );

    fgets(buff, 255, (FILE*)fp);
   printf("2: %s\n", buff );
   
   fgets(buff, 255, (FILE*)fp);
   printf("3: %s\n", buff );

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
            return true;
    return false;
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