#ifndef TFACE_CENTER_CONTROL_H
#define TFACE_CENTER_CONTROL_H

#include "cthread.h"
#include "followcam.h"

class followcam;

class tface_center_control : public CThread
{
public:
    tface_center_control(followcam *original_followcam);
    ~tface_center_control();
    void run(void*);
private:
     followcam *m_followcam;
     double deltatime, error, y_error, delta_error, y_delta_error, prev_error, y_prev_error, ci, y_ci, kd, ki, kp, y_kp, y_ki, y_kd, output, y_output;
     int value_pwm, y_value_pwm;
};

#endif // TFACE_CENTER_CONTROL_H
