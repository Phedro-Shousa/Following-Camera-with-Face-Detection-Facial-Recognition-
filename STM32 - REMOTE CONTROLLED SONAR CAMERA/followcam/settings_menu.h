#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include <QDialog>
#include "user_menu.h"
#include "mainwindow.h"

#define IDLE 0
#define MANUAL 1
#define AUTOMATIC_W_DETECT 2
#define AUTOMATIC_W_RECOGN 3

class USER_MENU;

namespace Ui {
class SETTINGS_MENU;
}

class SETTINGS_MENU : public QDialog
{
    Q_OBJECT

public:
    explicit SETTINGS_MENU(QWidget *parent = nullptr);
    ~SETTINGS_MENU();
    void define_parent(USER_MENU * original_usermenu);
private slots:
    void on_home_button_clicked();

    void on_detect_button_clicked();

    void on_recognize_button_clicked();

private:
    Ui::SETTINGS_MENU *ui;
    USER_MENU * m_user_menu;
};

#endif // SETTINGS_MENU_H
