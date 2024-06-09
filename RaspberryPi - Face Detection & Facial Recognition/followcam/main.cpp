#include "mainwindow.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videostab.hpp>
#include <opencv2/core/wimage.hpp>
#include <opencv2/photo/photo.hpp>
#include <ncurses.h>
#include <QApplication>
#include "followcam.h"


using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    followcam FOLLOWCAM;
    MainWindow w(nullptr, &FOLLOWCAM);
    w.show();
    FOLLOWCAM.updater.set_win_posx(w.Get_window_xpos());
    FOLLOWCAM.updater.set_win_posy(w.Get_window_ypos());
    return a.exec();
}
