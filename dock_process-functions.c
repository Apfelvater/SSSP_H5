#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_CORES 4

static int pcs = 0;
static int servers = 0;
static int monitors = 0;

void* process_H() {
    printf("process_H Critical Section Start <<\n");
    pcs += 3;
    servers += 1;
    monitors += 2;
    printf("process_H Critical Section End   >>\n");
    return 0;
}

void* process_D() {
    printf("process_D Critical Section Start <<\n");
    pcs -= 1;
    monitors -= 1;
    printf("process_D Critical Section End   >>\n");
    return 0;
}

void* process_S() {
    printf("process_S Critical Section Start <<\n");
    servers -= 1;
    printf("process_S Critical Section End   >>\n");
    return 0;
}


static char* execution = "HDSSSDHHDHSHHHD";

sem_t space, dock, wait4space, wait4items, pc, server, monitor;

int main() {    //TODO: Constraints beachten: anzahl geht unter 0, und über 24, (etc?)
    sem_init(&space, 1, 24);
    sem_init(&dock, 1, 1);
    sem_init(&wait4items, 1, 1);
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
        
        
        rc = pthread_create(&threads[i], NULL, process_X, NULL);
        i++;
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    
    for (int i = 0; i < n_ops; i++) {
        pthread_join(threads[i], NULL);
    }
        
    printf("Final Stock is:\n\tPCs: %d\n\tMonitors: %d\n\tServers: %d\n", pcs, monitors, servers);
    printf("Right values: (%d, %d, %d)\n", hc * 3 - dc, hc * 2 - dc, hc - sc); 
        
    return 0;
}
