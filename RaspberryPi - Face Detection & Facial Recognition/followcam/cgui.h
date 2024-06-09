#ifndef CGUI_H
#define CGUI_H
#include <QtGui>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class CGUI: public QObject
{
    Q_OBJECT
public:
    explicit CGUI(/*QObject *parent = 0*/);
    ~CGUI();
    void newimagerecord(const QImage& image);
    void newnumberoffaces(int num_of_faces);
    void set_win_posx(int positionx);
    int* get_win_posx();
    void set_win_posy(int positiony);
    int* get_win_posy();

    int window_posx;
    int window_posy;
signals:
    void displaynewimagerecord(const QImage&);
    void displaynumberoffaces(int num_of_faces);

private:
    int *xptr;
    int *yptr;
};

#endif // CGUI_H
