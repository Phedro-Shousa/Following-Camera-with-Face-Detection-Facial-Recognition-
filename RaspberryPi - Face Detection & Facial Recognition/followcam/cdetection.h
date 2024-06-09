#ifndef CDetection_H
#define CDetection_H
#include <opencv2/opencv.hpp>
#include <string>

using namespace cv;
using namespace std;

class CDetection
{
public:
    CDetection(const string classifier_path);
    vector<Rect_<int>> identifyFaces(Mat image);
    bool loadClassifier();
    bool classifierEmpty();

private:
    string m_classifier_path;
    CascadeClassifier m_classifier;
};

#endif // CDetection_H
