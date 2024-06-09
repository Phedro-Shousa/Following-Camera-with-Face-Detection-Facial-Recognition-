#include "followcam.h"

followcam::followcam()
{
    /*initialize threads*/
    m_tacquireframes = new tacquire_frames(this);
    m_tdetectface = new tdetect_face(this);
    m_trecognizeface = new trecognize_face(this);
    m_tgui = new tgui(this);
    m_tfacecentercontrol = new tface_center_control(this);
    m_tactuateservo = new tactuateservo(this);

    /*initialize semaphore*/
    sem_init(&sem_servo_manage, 0 , 0);

    mode = IDLE;
}

followcam::~followcam()
{
}

bool followcam::init_threads()
{
   m_threads.addThread((CThread*)m_tacquireframes,1);
   m_threads.addThread((CThread*)m_tdetectface,1);
   m_threads.addThread((CThread*)m_trecognizeface,1);
   m_threads.addThread((CThread*)m_tgui,1);
   m_threads.addThread((CThread*)m_tfacecentercontrol,1);
   m_threads.addThread((CThread*)m_tactuateservo,1);

   return m_threads.run();
}

