#include "settings_menu.h"
#include "ui_settings_menu.h"

SETTINGS_MENU::SETTINGS_MENU(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SETTINGS_MENU)
{
    ui->setupUi(this);
}

SETTINGS_MENU::~SETTINGS_MENU()
{
    delete ui;
}

void SETTINGS_MENU::on_home_button_clicked()
{
    this->close();
}

void SETTINGS_MENU::on_detect_button_clicked()
{
    m_user_menu->Set_Automatic_Mode_Type(AUTOMATIC_W_DETECT);
    this->close();
}


void SETTINGS_MENU::on_recognize_button_clicked()
{
    m_user_menu->Set_Automatic_Mode_Type(AUTOMATIC_W_RECOGN);
    this->close();
}

void SETTINGS_MENU::define_parent(USER_MENU * origin_usermenu){
    m_user_menu = origin_usermenu;
}
