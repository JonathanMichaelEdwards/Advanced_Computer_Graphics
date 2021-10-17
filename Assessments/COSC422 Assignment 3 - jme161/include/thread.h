#ifndef THREAD_H_
#define THREAD_H_


// box col
#define THREADS_BOX_COLL      1
#define THREADS_BOX_BOX_COLL  1
#define     NUM_THREADS     3


typedef struct _worker
{
    // int n;
    int start;          // start index of the worker thread
    int end;            // end index of the worker thread

    // pthread_mutex_t *lock;
    // pthread_t thread;

} Worker;


#endif // THREAD_H_

