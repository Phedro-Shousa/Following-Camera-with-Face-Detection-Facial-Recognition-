#include "tactuateservo.h"

tactuateservo::tactuateservo(followcam *original_followcam)
{
    m_followcam = original_followcam;
    m_followcam->x_position=MAX_X_POS/2;
    m_followcam->y_position=MIN_Y_POS/2;
}


tactuateservo::~tactuateservo(){
}

void tactuateservo::set_x_angle(int value, int *orig_value){

    if(value > MAX_X_POS){
        value = MAX_X_POS;
        *orig_value = MAX_X_POS;
    }else if(value < MIN_X_POS){
        value = MIN_X_POS;
        *orig_value = MIN_X_POS;
    }

    if(value < MAX_X_POS && value > MIN_X_POS){
    std::string pwm = "gpio pwm 23 ";
    pwm += std::to_string(value);
    system(pwm.c_str());
    }

}

void tactuateservo::set_y_angle(int value, int *orig_value){

    if(value > MAX_Y_POS){
        value = MAX_Y_POS;
        *orig_value = MAX_Y_POS;
    }
    if(value < MIN_Y_POS){
        value = MIN_Y_POS;
        *orig_value = MIN_Y_POS;
    }

    if(value <= MAX_Y_POS && value >= MIN_Y_POS){
    std::string pwm = "gpio pwm 26 ";
    pwm += std::to_string(value);
    system(pwm.c_str());
    }

}

void tactuateservo::run(void *param){
    while(1){
        sem_wait(&m_followcam->sem_servo_manage);


        std::string pwm = "gpio pwm 23 ";
        pwm += std::to_string(m_followcam->x_actuate_value);
        system(pwm.c_str());


        pwm = "gpio pwm 26 ";
        pwm += std::to_string(m_followcam->y_actuate_value);
        system(pwm.c_str());

    }
}
