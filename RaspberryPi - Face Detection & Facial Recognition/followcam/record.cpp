#include "record.h"
#include "ui_record.h"

record::record(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::record)
{
    ui->setupUi(this);

}

record::~record()
{
    delete ui;
}

void record::maina(MainWindow * origin_mainwindow, followcam *orig_followcam){

    m_mainwindow = origin_mainwindow;
    m_followcam = orig_followcam;
}

void record::update_screen()
{
    Mat record_frame;
    if(m_followcam->m_cam.isActive()){
        m_followcam->m_cam.captureFrame(record_frame);
        QImage image1= QImage((uchar*) record_frame.data, record_frame.cols, record_frame.rows, record_frame.step, QImage::/*Format_Grayscale8*/Format_RGB888);
        ui->label->setPixmap(QPixmap::fromImage(image1));
        ui->label->show();
    }
}
