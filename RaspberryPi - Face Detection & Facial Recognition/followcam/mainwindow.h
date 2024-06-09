#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cgui.h"
#include "followcam.h"
#include "settings_menu.h"

#define MAX_X_POS 510
#define MIN_X_POS 110
#define MAX_Y_POS 400
#define MIN_Y_POS 100
#define IDLE 0
#define MANUAL 1
#define AUTOMATIC_W_DETECT 2
#define AUTOMATIC_W_RECOGN 3

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class followcam;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0, followcam * orig_followcam=0);
    ~MainWindow();
    void Disable_manual_buttons();
    void Enable_manual_buttons();
    void Set_operation_mode(int orig_mode);
    int Get_window_xpos();
    int Get_window_ypos();
private slots:
    void addnewframerecord(const QImage&);

    void updatenumberoffaces(int num_of_faces);

    void on_settings_button_clicked();

    void on_arrowleft_button_clicked();

    void on_arrowright_button_clicked();

    void on_play_button_clicked();

    void on_arrowup_button_clicked();

    void on_arrowdown_button_clicked();

private:
    Ui::MainWindow *ui;
    followcam * m_followcam;
    QTimer *timer;
};
#endif // MAINWINDOW_H
