#ifndef ccamera_H
#define ccamera_H
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/imgproc.hpp>
#include <queue>
#include <string>
using namespace cv;
using namespace std;

class ccamera
{
public:
    ccamera();
    ccamera(int);
    ~ccamera();
    bool captureFrame(Mat&);
    bool isActive(void);
    bool open(void);
    void shutdown(void);
    VideoCapture get_camera();
private:
    VideoCapture m_cap;
    int m_device;
};


#endif
