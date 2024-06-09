#ifndef TACQUIRE_FRAMES_H
#define TACQUIRE_FRAMES_H
#include "cthread.h"
#include "ccamera.h"
#include "followcam.h"

class followcam;

class tacquire_frames : public CThread
{
public:
    tacquire_frames(followcam *original_followcam);
    ~tacquire_frames();
    void run(void*);

private:
    followcam *m_followcam;
};

#endif // TACQUIRE_FRAMES_H
