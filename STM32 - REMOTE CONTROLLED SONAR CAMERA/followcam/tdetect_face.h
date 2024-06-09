#ifndef TDETECT_FACE_H
#define TDETECT_FACE_H
#include "cthread.h"
#include "cdetection.h"
#include "followcam.h"

#define IDLE 0
#define MANUAL 1
#define AUTOMATIC_W_DETECT 2
#define AUTOMATIC_W_RECOGN 3

class followcam;

class tdetect_face : public CThread
{
public:
    tdetect_face(followcam *original_followcam);
    ~tdetect_face();
    void run(void*);

private:
    followcam * m_followcam;
};

#endif // TDETECT_FACE_H
