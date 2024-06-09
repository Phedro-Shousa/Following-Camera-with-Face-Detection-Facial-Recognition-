#ifndef FOLLOWCAM_H
#define FOLLOWCAM_H

#include"cdetection.h"
#include"ccamera.h"
#include"cgui.h"
#include "mainwindow.h"

#include"tacquire_frames.h"
#include"tdetect_face.h"
#include"trecognize_face.h"
#include"tactuateservo.h"
#include"tface_center_control.h"
#include"tgui.h"
#include <semaphore.h>
#include"cthreads.h"
#include<mutex>

#define SIZE_OF_FRAMES_WIDTH 640
#define SIZE_OF_FRAMES_LENGHT 480

#define IDLE 0
#define MANUAL 1
#define AUTOMATIC_W_DETECT 2
#define AUTOMATIC_W_RECOGN 3

#define MAX_X_POS 510
#define MIN_X_POS 110
#define MAX_Y_POS 400
#define MIN_Y_POS 100

class tacquire_frames;
class tdetect_face;
class trecognize_face;
class tactuateservo;
class tgui;
class tface_center_control;

typedef struct info_detection{
    Mat last_frame, gray_frame_recized;
    vector<cv::Rect_<int>> vec_of_faces;
}info_tdectect_face;

class followcam
{
public:
    followcam();
    ~followcam();

    bool init_threads();

//objects threads
    tacquire_frames *m_tacquireframes;
    tdetect_face *m_tdetectface;
    trecognize_face *m_trecognizeface;
    tactuateservo *m_tactuateservo;
    tgui *m_tgui;
    tface_center_control *m_tfacecentercontrol;
    CThreads m_threads;

//other objects
    ccamera m_cam;
    CGUI updater;

//variables
    Mat last_framecaptured;
    int mode;
    int x_position;
    int y_position;
    double x_deltaerror;
    double y_deltaerror;
    int x_actuate_value;
    int y_actuate_value;
//struct's of threads
    info_tdectect_face data_tdetect_face;

//condition variables
    pthread_cond_t condition_img_acquired = PTHREAD_COND_INITIALIZER;
    pthread_cond_t condition_img_detected = PTHREAD_COND_INITIALIZER;
    pthread_cond_t condition_print_image = PTHREAD_COND_INITIALIZER;
    pthread_cond_t condition_face_center_control = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t condition_img_acquired_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t condition_img_detected_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t condition_print_image_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t condition_face_center_control_mutex = PTHREAD_MUTEX_INITIALIZER;

//mutexes
    pthread_mutex_t mutex_operation_mode = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mutex_lastframecap = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mutex_detection_data = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mutex_face_center_control = PTHREAD_MUTEX_INITIALIZER;

    sem_t sem_servo_manage;
};

#endif // FOLLOWCAM_H
