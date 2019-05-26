#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_CORES 4

static int pcs = 0;
static int servers = 0;
static int monitors = 0;

sem_t space, dock, wait4space, wait4items, pc, server, monitor;

void* process_H(int* id) {
    //printf("H_Process-Truck %d arrives!\n", *id);
    sem_wait(&wait4space);
        for (int i = 0; i < 5; i++) { sem_wait(&space); }
    sem_post(&wait4space);
    sem_wait(&dock);
        printf("H_Process#%d Critical Section Start <\n", *id);
        pcs += 2;
        servers += 1;
        monitors += 2;
        //printf("PCs:%d,SVs:%d,MTs:%d\n", pcs, servers, monitors);
        printf("H_Process#%d Critical Section End   >\n", *id);
    sem_post(&dock);
    sem_post(&server);
    sem_post(&pc);
    //sem_post(&pc);    *DOING ONLY 2 PC LOADS*
    sem_post(&pc);
    sem_post(&monitor);
    sem_post(&monitor);
    //printf("H_Process-Truck %d drives away...\n", *id);

    pthread_exit(0);
    return 0;
}

void* process_D(int* id) {
    //printf("D_Process-Truck %d arrives!\n", *id);
    sem_wait(&wait4items);
        sem_wait(&pc);
        sem_wait(&monitor);
    sem_post(&wait4items);
    sem_wait(&dock);
        printf("D_Process#%d Critical Section Start <\n", *id);
        pcs -= 1;
        monitors -= 1;
        //printf("PCs:%d,SVs:%d,MTs:%d\n", pcs, servers, monitors);
        printf("D_Process#%d Critical Section End   >\n", *id);
    sem_post(&dock);
    sem_post(&space);
    sem_post(&space);
    //printf("D_Process-Truck %d drives away...\n", *id);

    pthread_exit(0);
    return 0;
}

void* process_S(int* id) {
    //printf("S_Process-Truck %d arrives!\n", *id);
    sem_wait(&server);
    sem_wait(&dock);
        printf("S_Process#%d Critical Section Start <\n", *id);
        servers -= 1;
        //printf("PCs:%d,SVs:%d,MTs:%d\n", pcs, servers, monitors);
        printf("S_Process#%d Critical Section End   >\n", *id);
    sem_post(&dock);
    sem_post(&space);
    //printf("S_Process-Truck %d drives away...\n", *id);

    pthread_exit(0);
    return 0;
}


static char* execution = "HDDS";


int main(int argc, char* argv[]) { 

    if (argc < 2) {
        printf("Usage:\ndock.o *execution-order*\ne.g.: dock.o HDSHDSSDDDHSD\n");
        return 1;
    }
    execution = argv[1];

    sem_init(&space, 1, 24);
    sem_init(&dock, 1, 1);
    sem_init(&wait4space, 1, 1);
    sem_init(&wait4items, 1, 1);
    sem_init(&pc, 1, 0);
    sem_init(&server, 1, 0);
    sem_init(&monitor, 1, 0);
    
    int n_ops = strlen(execution);  //Amount of operations to be done
    pthread_t threads[n_ops];
    
    int hc = 0, dc = 0, sc = 0;
    
    void* process_X = (void*) NULL;
    int rc;
    int i = 0;
    while(execution[i]) {
        printf("In main: creating thread %d\n", i);
        switch(execution[i]) {
            case 72: process_X = process_H; hc++; break;
            case 68: process_X = process_D; dc++; break;
            case 83: process_X = process_S; sc++; break;
            default: ;;
        }
        
        
        rc = pthread_create(&threads[i], NULL, process_X, &i);
        i++;
    }

    if (hc * 2 - dc < 0 || hc * 2 - dc < 0 || hc - sc < 0) {
        printf("Some Trucks will endlessly wait for items. Exiting...\n");
        exit(-1);
    }
    for (int i = 0; i < n_ops; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final Stock is:\n\tPCs: %d\n\tMonitors: %d\n\tServers: %d\n", pcs, monitors, servers);
    printf("Right values: (%d, %d, %d)\n", hc * 2 - dc, hc * 2 - dc, hc - sc); 

    pthread_exit(0);

    return 0;
}
