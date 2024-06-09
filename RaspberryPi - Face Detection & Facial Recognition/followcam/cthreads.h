#ifndef CTHREADS_H
#define CTHREADS_H
#include <list>
#include "cthread.h"
using namespace std;
class CThreads
{
public:
    CThreads();
    ~CThreads();
    bool addThread(CThread* thread = nullptr,int prio = 2);
    bool run(void);
    int join(void);
private:
    list <CThread*> m_list_threads;
};

#endif // CTHREADS_H
