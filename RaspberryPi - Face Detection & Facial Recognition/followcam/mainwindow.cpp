#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "user_menu.h"

#include <QPixmap>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent, followcam * orig_followcam)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_followcam = orig_followcam;
    connect( &m_followcam->updater , SIGNAL(displaynewimagerecord(QImage)), this, SLOT(addnewframerecord(QImage)));
    connect( &m_followcam->updater ,SIGNAL(displaynumberoffaces(int)), this, SLOT(updatenumberoffaces(int)));
    orig_followcam->init_threads();
    Disable_manual_buttons();
    ui->label->setStyleSheet("color: white;");
    ui->label_2->setStyleSheet("color: white;");
    ui->label_6->setStyleSheet("color: white;");
    ui->label_7->setStyleSheet("color: white;");
    ui->label_8->setStyleSheet("color: white;");
    m_followcam->x_position = MAX_X_POS/2;
    m_followcam->y_position = MAX_Y_POS/2;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addnewframerecord(const QImage& frame)
{
   ui->label_record->setPixmap(QPixmap::fromImage(frame/*.scaled(SIZE_OF_FRAMES_WIDTH,SIZE_OF_FRAMES_LENGHT,Qt::KeepAspectRatio)*/));
}

void MainWindow::updatenumberoffaces(int num_of_faces)
{
   ui->number_of_faces_lcd->display(num_of_faces);
}

void MainWindow::Disable_manual_buttons(){
    ui->arrowleft_button->setDisabled(true);
    ui->arrowright_button->setDisabled(true);
    ui->arrowup_button->setDisabled(true);
    ui->arrowdown_button->setDisabled(true);
}

void MainWindow::Enable_manual_buttons(){
    ui->arrowleft_button->setEnabled(true);
    ui->arrowright_button->setEnabled(true);
    ui->arrowup_button->setEnabled(true);
    ui->arrowdown_button->setEnabled(true);
}

void MainWindow::on_settings_button_clicked()
{
    USER_MENU settings_window(nullptr, this);
    settings_window.exec();
}

void MainWindow::Set_operation_mode(int orig_mode){

     if(orig_mode == IDLE){
         QPixmap idle(":etc/bimgs_qt/LIGHT_ON.png");
         ui->LIGHT_IDLE_MODE->setPixmap(idle.scaled(72, 72, Qt::KeepAspectRatio));
         QPixmap warming(":etc/bimgs_qt/WARMING.jpg");
         ui->label_record->setPixmap(warming.scaled(ui->label_record->width(),481 , Qt::KeepAspectRatio));
         QPixmap idle_off(":etc/bimgs_qt/LIGHT_OFF.png");
         ui->LIGHT_MANUAL_MODE->setPixmap(idle_off.scaled(72, 72, Qt::KeepAspectRatio));
         ui->LIGHT_DETECTION_MODE->setPixmap(idle_off.scaled(72, 72, Qt::KeepAspectRatio));
         ui->LIGHT_RECOGNITION_MODE->setPixmap(idle_off.scaled(72, 72, Qt::KeepAspectRatio));
         Disable_manual_buttons();
     }else if( orig_mode == MANUAL){
         m_followcam->m_cam.shutdown();
         QPixmap manual(":etc/bimgs_qt/LIGHT_ON.png");
         ui->LIGHT_MANUAL_MODE->setPixmap(manual.scaled(72, 72, Qt::KeepAspectRatio));
         QPixmap manual_off(":etc/bimgs_qt/LIGHT_OFF.png");
         ui->LIGHT_IDLE_MODE->setPixmap(manual_off.scaled(72, 72, Qt::KeepAspectRatio));
         ui->LIGHT_DETECTION_MODE->setPixmap(manual_off.scaled(72, 72, Qt::KeepAspectRatio));
         ui->LIGHT_RECOGNITION_MODE->setPixmap(manual_off.scaled(72, 72, Qt::KeepAspectRatio));
     }else if(orig_mode == AUTOMATIC_W_DETECT){
         Disable_manual_buttons();
         m_followcam->m_cam.shutdown();
         QPixmap detect(":etc/bimgs_qt/LIGHT_ON.png");
         ui->LIGHT_DETECTION_MODE->setPixmap(detect.scaled(72, 72, Qt::KeepAspectRatio));
         QPixmap detect_off(":etc/bimgs_qt/LIGHT_OFF.png");
         ui->LIGHT_IDLE_MODE->setPixmap(detect_off.scaled(72, 72, Qt::KeepAspectRatio));
         ui->LIGHT_MANUAL_MODE->setPixmap(detect_off.scaled(72, 72, Qt::KeepAspectRatio));
         ui->LIGHT_RECOGNITION_MODE->setPixmap(detect_off.scaled(72, 72, Qt::KeepAspectRatio));
     }else if(orig_mode == AUTOMATIC_W_RECOGN){
         Disable_manual_buttons();



         
         m_followcam->m_cam.shutdown();
         QPixmap recogn(":etc/bimgs_qt/LIGHT_ON.png");
         ui->LIGHT_RECOGNITION_MODE->setPixmap(recogn.scaled(72, 72, Qt::KeepAspectRatio));
         QPixmap recogn_off(":etc/bimgs_qt/LIGHT_OFF.png");
         ui->LIGHT_IDLE_MODE->setPixmap(recogn_off.scaled(72, 72, Qt::KeepAspectRatio));
         ui->LIGHT_MANUAL_MODE->setPixmap(recogn_off.scaled(72, 72, Qt::KeepAspectRatio));
         ui->LIGHT_DETECTION_MODE->setPixmap(recogn_off.scaled(72, 72, Qt::KeepAspectRatio));
     }

    m_followcam->mode = orig_mode;
}

void MainWindow::on_arrowleft_button_clicked()
{
    (m_followcam->x_position) -= 30;
    m_followcam->m_tactuateservo->set_x_angle(m_followcam->x_position, &m_followcam->x_position);
}

void MainWindow::on_arrowright_button_clicked()
{
    m_followcam->x_position += 30;
    m_followcam->m_tactuateservo->set_x_angle(m_followcam->x_position, &m_followcam->x_position);
}

int MainWindow::Get_window_xpos(){
    return ui->label_record->x();
}

int MainWindow::Get_window_ypos(){
    return ui->label_record->y();
}

void MainWindow::on_play_button_clicked()
{
    Set_operation_mode(IDLE);
}



void MainWindow::on_arrowup_button_clicked()
{
    m_followcam->y_position -= 40;
    m_followcam->m_tactuateservo->set_y_angle(m_followcam->y_position, &m_followcam->y_position);
}

void MainWindow::on_arrowdown_button_clicked()
{
    m_followcam->y_position += 40;
    m_followcam->m_tactuateservo->set_y_angle(m_followcam->y_position, &m_followcam->y_position);
}
