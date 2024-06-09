#include "cthread.h"

#include <QDebug>

bool CThread:: m_run_flag = false;
CThread::CThread()
{
}
CThread::~CThread()
{

}
bool CThread::create()
{
    m_mutex_run_flag = PTHREAD_MUTEX_INITIALIZER;
    return (pthread_create(&m_thread, NULL, &thread_func, (void*)this) == 0);
}
void* CThread::thread_func(void* param)
{
    ((CThread*)param)->run(nullptr);
    return nullptr;
}
bool CThread::check_run_state()
{
    bool state;
    pthread_mutex_lock(&m_mutex_run_flag);
    state = m_run_flag;
    pthread_mutex_unlock(&m_mutex_run_flag);
    return state;
}
void CThread::shutdown()
{
    pthread_mutex_lock(&m_mutex_run_flag);
    m_run_flag = false;
    pthread_mutex_unlock(&m_mutex_run_flag);
}

int CThread::join()
{
    return 1;
    return pthread_join(m_thread, NULL);
}


void CThread::setup_thread(int prio)
{
    int rr_min_prio, rr_max_prio;
    pthread_attr_init(&m_attr);
    pthread_attr_setschedpolicy(&m_attr, SCHED_RR);
    rr_min_prio = sched_get_priority_min(SCHED_RR);
    rr_max_prio = sched_get_priority_max(SCHED_RR);
    m_thread_param.sched_priority = prio;
    pthread_attr_setschedparam(&m_attr, &m_thread_param);
    pthread_attr_setinheritsched(&m_attr, PTHREAD_EXPLICIT_SCHED);
}

