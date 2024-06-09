#include "cgui.h"
#include "mainwindow.h"

CGUI::CGUI(/*QObject *parent*/) //: QObject(parent)
{
    //window_posx = MainWindow::Get_window_xpos();
    xptr = &window_posx;
    yptr = &window_posy;
}

CGUI::~CGUI()
{
}

void CGUI::newimagerecord(const QImage &frame)
{
    emit displaynewimagerecord(frame.copy());
}


void CGUI::newnumberoffaces(int num_of_faces){

    emit displaynumberoffaces(num_of_faces);
}

void CGUI::set_win_posx(int positionx){
    *xptr = positionx;
}

void CGUI::set_win_posy(int positiony){
    *yptr = positiony;
}

int* CGUI::get_win_posx(){
    return xptr;
}

int* CGUI::get_win_posy(){
    return yptr;
}
