#include "ccamera.h"
#include <QThread>

ccamera::ccamera():m_device(0)
{
}
ccamera::ccamera(int device): m_device(device)
{
    //system("modprobe bcm2835-v4l2");
   // system("kill 222");
   // system("v4l2-ctl --set-ctrl=auto_exposure=1");
    //m_cap.set(CV_CAP_PROP_FRAME_WIDTH, 600);
}

ccamera::~ccamera()
{
    /*realease camera*/
    m_cap.release();
}

bool ccamera::open()
{
    /* open video device*/
    return m_cap.open(m_device);
}

void ccamera::shutdown()
{
   m_cap.release();
}

bool ccamera::captureFrame(cv::Mat& image) //## testar capture frame sem abrir a camera
{
    Mat frame;
    if( m_cap.isOpened())
    {
       m_cap.read(frame);
       image = frame.clone();
       return true;
    }
    return false;
}

bool ccamera::isActive(void)
{
    return m_cap.isOpened();
}

VideoCapture ccamera::get_camera(){

    return m_cap;

}
