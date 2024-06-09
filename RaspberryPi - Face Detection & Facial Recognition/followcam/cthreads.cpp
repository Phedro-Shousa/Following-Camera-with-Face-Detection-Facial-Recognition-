#include "cthreads.h"

#include <QDebug>

CThreads::CThreads()
{

}
CThreads::~CThreads()
{

}
bool CThreads::addThread(CThread* thread,int prio)
{
    if(thread != nullptr)
    {
        thread->setup_thread(prio);
        m_list_threads.push_back(thread);
        return true;
    }
    return false;
}

bool CThreads::run(void)
{
    for(list<CThread*>::iterator it = m_list_threads.begin(); it != m_list_threads.end(); ++it)
    {
        if (!(*it)->create())
        {
            return false;
        }
    }
    return true;
}

int CThreads::join()
{
    for(list<CThread*>::iterator it = m_list_threads.begin(); it != m_list_threads.end(); ++it)
    {
        if ((*it)->join())
        {
            return false;
        }
    }
    return true;
}
