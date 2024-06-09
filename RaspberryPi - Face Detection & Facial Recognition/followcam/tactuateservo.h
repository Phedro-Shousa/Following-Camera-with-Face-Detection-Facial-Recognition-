#ifndef TACTUATESERVO_H
#define TACTUATESERVO_H
#include "cthread.h"
#include "followcam.h"

#define MAX_X_POS 510
#define MIN_X_POS 110

#define MAX_Y_POS 400
#define MIN_Y_POS 100

class followcam;

class tactuateservo: public CThread
{
public:
    tactuateservo(followcam *original_followcam);
    ~tactuateservo();
    void set_x_angle(int value, int *orig_value);
    void set_y_angle(int value, int *orig_value);
    void run(void*);

private:
    followcam * m_followcam;
};

#endif // TACTUATESERVO_H
