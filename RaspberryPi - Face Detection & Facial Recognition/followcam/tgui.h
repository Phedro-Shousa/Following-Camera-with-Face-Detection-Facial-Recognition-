#ifndef TGUI_H
#define TGUI_H

#include "followcam.h"
#include "cthread.h"

class tgui : public CThread
{
public:
    tgui(followcam *original_followcam);
    ~tgui();
    void run(void*);

private:
     followcam *m_followcam;
};

#endif // TGUI_H
