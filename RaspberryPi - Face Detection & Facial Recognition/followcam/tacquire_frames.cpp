#include "tacquire_frames.h"
#include "followcam.h"
#include "pthread.h"
#include "unistd.h"

#define MAX_SIZE_OF_FRAMES_CAPTURED 10
#define IDLE 0
#define MANUAL 1
#define AUTOMATIC_W_DETECT 2
#define AUTOMATIC_W_RECOGN 3

tacquire_frames::tacquire_frames(followcam* origin_followcam)
{
    m_followcam=origin_followcam;
}

tacquire_frames::~tacquire_frames()
{}

void tacquire_frames::run(void* param)
{

 Mat current_frame;
    while(1){
      //cout << "num: " << m_followcam->mode << endl;
      pthread_mutex_lock(&(m_followcam->mutex_operation_mode));
      switch (m_followcam->mode){
            case IDLE:
            usleep(100000);
            break;

            case MANUAL:
            //cout << "MANUAL MODE" << endl;
            if(m_followcam->m_cam.isActive()){
            m_followcam->m_cam.captureFrame(current_frame);
            }else{
                m_followcam->m_cam.open();
                while(!(m_followcam->m_cam.isActive())){}
                m_followcam->m_cam.get_camera().set(CV_CAP_PROP_FRAME_WIDTH, SIZE_OF_FRAMES_WIDTH/2);
                m_followcam->m_cam.get_camera().set(CV_CAP_PROP_FRAME_HEIGHT, SIZE_OF_FRAMES_LENGHT/2);
                m_followcam->m_cam.captureFrame(current_frame);
            }

            pthread_mutex_lock(&(m_followcam->mutex_lastframecap));
            m_followcam->last_framecaptured = current_frame;
            pthread_mutex_unlock(&(m_followcam->mutex_lastframecap));

            pthread_cond_signal( &(m_followcam->condition_print_image));
            break;

            case AUTOMATIC_W_DETECT:
            //cout << "DETECT MODE" << endl;
            if(m_followcam->m_cam.isActive()){
                m_followcam->m_cam.captureFrame(current_frame);
            }else{
                m_followcam->m_cam.open();
                while(!(m_followcam->m_cam.isActive())){}
                m_followcam->m_cam.get_camera().set(CV_CAP_PROP_FRAME_WIDTH, SIZE_OF_FRAMES_WIDTH/2);
                m_followcam->m_cam.get_camera().set(CV_CAP_PROP_FRAME_HEIGHT, SIZE_OF_FRAMES_LENGHT/2);
                m_followcam->m_cam.captureFrame(current_frame);
            }

            pthread_mutex_lock(&(m_followcam->mutex_lastframecap));
            m_followcam->last_framecaptured = current_frame;
            pthread_mutex_unlock(&(m_followcam->mutex_lastframecap));

            pthread_cond_signal( &(m_followcam->condition_img_acquired));
            break;

            case AUTOMATIC_W_RECOGN:
                //cout << "reco MODE" << endl;
            if(m_followcam->m_cam.isActive()){
                m_followcam->m_cam.captureFrame(current_frame);
            }else{
                m_followcam->m_cam.open();
                while(!(m_followcam->m_cam.isActive())){}
                m_followcam->m_cam.get_camera().set(CV_CAP_PROP_FRAME_WIDTH, SIZE_OF_FRAMES_WIDTH/2);
                m_followcam->m_cam.get_camera().set(CV_CAP_PROP_FRAME_HEIGHT, SIZE_OF_FRAMES_LENGHT/2);
                m_followcam->m_cam.captureFrame(current_frame);
            }

            pthread_mutex_lock(&(m_followcam->mutex_lastframecap));
            m_followcam->last_framecaptured = current_frame;
            pthread_mutex_unlock(&(m_followcam->mutex_lastframecap));

            pthread_cond_signal( &(m_followcam->condition_img_acquired));
      }
      pthread_mutex_unlock(&(m_followcam->mutex_operation_mode));
   }
}
