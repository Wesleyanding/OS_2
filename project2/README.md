# Project 2

## Building

Command Line

`make` to build. An executable called  `reservations` will be produced. 

## Files

`reservations.c`: The main code to launch the project

## Data
`seat_taken` - an array of seats

## Functions

`lock` : pthread_mutex_t lock initilizer

* `reserve_seat()` : Function that takes n and reserves a seat in the seat_taken array
    * `pthread_mutex_lock()` : Lock the mutex
    * `is_free()` : Function that takes n and returns true is the seat is available
    *  `pthread_mutex_unlock()` : Unlock the mutex

* `free_seat()` : Function that takes n and frees a seat in the seat_taken array
    * `pthread_mutex_lock()` : Lock the mutex
    * `is_free` : Function that takes n and returns true is the seat is available
    *  `pthread_mutex_unlock()` : Unlock the mutex

* `verify_seat_count()` : Function that counts all the taken seats in the seat_taken array, compares the count and the seat_count global variable and returns true if they are the same. 
    * `pthread_mutex_lock()` : Lock the mutex
    *  `pthread_mutex_unlock()` : Unlock the mutex

* `*seat_broker()` : Function to buy and sell seats

* `main()` : Main function that produces the threads, sets the seat_count, broker_count, transaction_count and seat_taken array. 
Launches the threads and waits for them to complete. 

## Notes
    Used in class notes!
