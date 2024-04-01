#include <stdio.h>
#include <pthread.h>

// Function to be executed by the thread
void *thread_function(void *arg)
{
    char *message = (char *)arg;
    for (int i = 0; i < 5; i++)
    {
        printf("%s: %d\n", message, i);
    }

    return NULL;
}

int main()
{
    pthread_t thread1;
    pthread_t thread2;
    char *message1 = "Thread 1";
    char *message2 = "Thread 2";

    puts("Launching threads");

    // Create the first thread
    pthread_create(&thread1, NULL, thread_function, message1);

    // Create the second thread
    pthread_create(&thread2, NULL, thread_function, message2);

    // Wait for the threads to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    puts("Threads complete!");
    return 0;
}