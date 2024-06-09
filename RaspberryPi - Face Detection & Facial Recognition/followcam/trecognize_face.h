#ifndef TRECOGNIZE_FACE_H
#define TRECOGNIZE_FACE_H
#include "followcam.h"
#include "cthread.h"
#include "cprediction.h"

#define IDLE 0
#define MANUAL 1
#define AUTOMATIC_W_DETECT 2
#define AUTOMATIC_W_RECOGN 3

class followcam;

class trecognize_face : public CThread
{
public:
    trecognize_face(followcam *original_followcam);
    ~trecognize_face();
    void run(void*);
private:
    followcam * m_followcam;
    Ptr<FaceRecognizer> model;
    CPrediction prediction;
};

#endif // TRECOGNIZE_FACE_H
