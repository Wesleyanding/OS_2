#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "eventbuf.h"

// These will be initialized in main() from the command line.
int producer_number;
int consumer_number;
int event_number;
int outstanding_number;

// Event buffer
struct eventbuf *eb;

sem_t *sem_producer;
sem_t *sem_consumer;

sem_t *sem_open_temp(const char *name, int value)
{
    sem_t *sem;

    // Create the semaphore
    if ((sem = sem_open(name, O_CREAT, 0600, value)) == SEM_FAILED)
        return SEM_FAILED;

    // Unlink it so it will go away after this process exits
    if (sem_unlink(name) == -1)
    {
        sem_close(sem);
        return SEM_FAILED;
    }

    return sem;
}

void *producer(void *arg)
{

    int *id = arg;

    for (int i = 0; i < event_number; i++)
    {
        // Wait for the event buffer to have space
        sem_wait(sem_producer);

        int event_num = i * 100 + i;
        // Print adding event, with event number
        printf("P%d: adding event %d\n", *id, event_num);

        // Add the event to the buffer
        eventbuf_add(eb, event_num);

        // Notify the consumers that there is an event
        sem_post(sem_consumer);
    }
    // print that it is exiting
    printf("P%d: exiting\n", *id);
    return NULL;
}

void *consumer(void *arg)
{

    int *id = arg;

    for (int i = 0; i < event_number; i++)
    {
        // Wait for an event to be available
        sem_wait(sem_consumer);

        int event = eventbuf_get(eb);

        // check if eventbuf is empty
        if (eventbuf_empty(eb))
        {
            // print that it is exiting
            printf("C%d: exiting\n", *id);
            // Unlock the mutex
            sem_post(sem_producer);
            return NULL;
        }
        // Print consuming event, with event number
        printf("C%d: got event %d\n", *id, event);

        // Notify the producers that there is space in the buffer
        sem_post(sem_producer);
    }

    // print that it is exiting
    printf("C%d: exiting\n", *id);
    return NULL;
}

int main(int argc, char *argv[])
{

    if (argc != 5)
    {
        fprintf(stderr, "usage: pc producer_count consumer_count event_count outstanding_count\n");
        exit(1);
    }

    producer_number = atoi(argv[1]);
    consumer_number = atoi(argv[2]);
    event_number = atoi(argv[3]);
    outstanding_number = atoi(argv[4]);

    // Create the mutex
    sem_producer = sem_open_temp("program-Producer-mutex", event_number);
    sem_consumer = sem_open_temp("program-Consumer-mutex", outstanding_number);

    // Create the event buffer
    eb = eventbuf_create();

    // Allocate the thread for all producers
    pthread_t *producer_thread = calloc(producer_number, sizeof *producer_thread);

    // Allocate the thread for all consumers
    pthread_t *consumer_thread = calloc(consumer_number, sizeof *consumer_thread);

    // allocate thread ID array for all producers
    int *producer_id = calloc(producer_number, sizeof *producer_id);

    // Start the producer threads
    for (int i = 0; i < producer_number; i++)
    {
        producer_id[i] = i;
        pthread_create(producer_thread + i, NULL, producer, producer_id + i);
    }

    // Allocate thread ID array for all consumers
    int *consumer_id = calloc(consumer_number, sizeof *consumer_id);

    // Start the consumer threads
    for (int i = 0; i < consumer_number; i++)
    {
        consumer_id[i] = i;
        pthread_create(consumer_thread + i, NULL, consumer, consumer_id + i);
    }

    // Wait for all producer threads to complete
    for (int i = 0; i < producer_number; i++)
    {
        pthread_join(producer_thread[i], NULL);
    }
    // Wait for all consumer threads to complete
    for (int i = 0; i < consumer_number; i++)
    {
        pthread_join(consumer_thread[i], NULL);
    }

    // Close the producer mutex
    sem_close(sem_producer);

    // Close the consumer mutex
    sem_close(sem_consumer);

    // Free the event buffer
    eventbuf_free(eb);

    return 0;
}