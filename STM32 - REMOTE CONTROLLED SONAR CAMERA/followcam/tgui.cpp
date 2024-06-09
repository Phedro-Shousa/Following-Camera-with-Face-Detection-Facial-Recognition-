#include "tgui.h"
#include "unistd.h"
tgui::tgui(followcam *original_followcam)
{
    m_followcam = original_followcam;
}

tgui::~tgui(){
}

void tgui::run(void *){

    while(1){

        pthread_cond_wait(&(m_followcam->condition_print_image), &(m_followcam->condition_print_image_mutex));
        pthread_mutex_lock(&(m_followcam->mutex_lastframecap));
        cv :: resize(m_followcam->last_framecaptured, m_followcam->last_framecaptured , Size(SIZE_OF_FRAMES_WIDTH,SIZE_OF_FRAMES_LENGHT), 1.0, 1.0, INTER_CUBIC);
        m_followcam->updater.newimagerecord(QImage(m_followcam->last_framecaptured.data, m_followcam->last_framecaptured.cols, m_followcam->last_framecaptured.rows, m_followcam->last_framecaptured.step, QImage::Format_RGB888));
        pthread_mutex_unlock(&(m_followcam->mutex_lastframecap));
        usleep(200000);
    }
}
