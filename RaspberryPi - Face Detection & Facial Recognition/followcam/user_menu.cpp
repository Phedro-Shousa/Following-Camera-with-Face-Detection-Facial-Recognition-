#include "user_menu.h"
#include "ui_user_menu.h"

USER_MENU::USER_MENU(QWidget *parent ,MainWindow * origin_mainwindow) :
    QDialog(parent),
    ui(new Ui::USER_MENU)
{
    ui->setupUi(this);
    m_mainwindow = origin_mainwindow;
}

USER_MENU::~USER_MENU()
{
    delete ui;
}

void USER_MENU::on_home_button_clicked()
{
    this->close();
}

void USER_MENU::on_automatic_button_clicked()
{
    SETTINGS_MENU window_settings(nullptr);
    window_settings.define_parent(this);
    window_settings.exec();
    this->close();
    //in case of automatic mode beeing choosed the manual mode is not necessary so we disconnect the buttons:
    m_mainwindow->Disable_manual_buttons();
}

void USER_MENU::on_manual_button_clicked()
{
    //put MANUAL_MODE on
    m_mainwindow->Enable_manual_buttons();
    m_mainwindow->Set_operation_mode(MANUAL);
    this->close();
}

void USER_MENU::Set_Automatic_Mode_Type(int mode){

    m_mainwindow->Set_operation_mode(mode);

}
