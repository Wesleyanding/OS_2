# Project 3

## Building

Command line

`gcc -Wall -Wextra -Werror -o pc pc.c eventbuf.c` to build. An executable called `pc` will be produced. 

## Files

`pc.c` : The main code to launch the project
`eventbuf.c` : The code used for a FIFO queue
`eventbuf.h` : Shows example usage for eventbuf.c 

## Data

`eventbuf` - a FIFO queue used for storing events

## Functions

`sem_producer` : semaphore initializer for the producer thread
`sem_consumer` : semaphore initializer for the consumer thread

`sem_open_temp()` : Function that takes a name and a value and creates a semaphore. Given in exploration and project description

* `producer()` : Function that produces the events, adding them to the eventbuf. 
    * `sem_wait()` : Function that waits on specific semaphore.
    * `eventbuf_add()` : Function that adds event number to eventbuf.
    * `sem_post()` : Function that changes semaphore.

* `consumer()` : Function that consumes the events, removing them from the eventbuf.
    * `sem_wait()` : Function that waits on specific semaphore.
    * `eventbuf_empty()` : Function used to check if eventbuf is empty. 
    * `sem_post()` : Function that changes the semaphore.

* `main()` : Main Function that produces the threads, sets the producer_number, consumer_number, event_number, and outstanding_number. 
    * `sem_open_temp()` : Function used to create semaphore mutex.
    * `eventbuf_create()` : Function used to create evenbuf FIFO queue.
    * `eventbuf_free()` : Function to free the event buffer. 

## Notes
    Used in class notes. 
