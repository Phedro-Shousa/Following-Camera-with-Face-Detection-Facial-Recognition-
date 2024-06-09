#include "cdetection.h"

CDetection::CDetection(const string classifier_path):m_classifier_path(classifier_path)
{

}

vector<Rect_<int> > CDetection::identifyFaces(Mat frame_gray)
{
    vector<Rect_<int>> faces;
    if(!frame_gray.empty() && !m_classifier.empty())
    {
        m_classifier.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(50, 50));
    }
    return faces;
}

bool CDetection::loadClassifier()
{
    return m_classifier.load(m_classifier_path);
}

bool CDetection::classifierEmpty()
{
    return m_classifier.empty();
}
