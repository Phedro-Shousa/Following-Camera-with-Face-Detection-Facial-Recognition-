#ifndef USER_MENU_H
#define USER_MENU_H

#include <QDialog>
#include "mainwindow.h"


#define IDLE 0
#define MANUAL 1
#define AUTOMATIC_W_DETECT 2
#define AUTOMATIC_W_RECOGN 3

class MainWindow;

namespace Ui {
class USER_MENU;
}

class USER_MENU : public QDialog
{
    Q_OBJECT

public:
    explicit USER_MENU(QWidget *parent , MainWindow *origin_mainwindow);
    ~USER_MENU();
    void Set_Automatic_Mode_Type(int mode);
private slots:
    void on_home_button_clicked();

    void on_automatic_button_clicked();

    void on_manual_button_clicked();

private:
    Ui::USER_MENU *ui;
    MainWindow * m_mainwindow;
};

#endif // USER_MENU_H
