#ifndef CTHREAD_H
#define CTHREAD_H
#include <pthread.h>
#include <mutex>

class CThread
{
public:
    CThread();
    virtual ~CThread() = 0;
    bool create();
    void shutdown();
    bool check_run_state();
    void setup_thread(int);
    int join();
protected:
    virtual void run(void*) = 0;
private:
    static void* thread_func(void*);
    static bool m_run_flag;
    pthread_t m_thread;
    pthread_mutex_t m_mutex_run_flag;
    pthread_attr_t m_attr;
    struct sched_param m_thread_param;
};

#endif // CTHREAD_H
