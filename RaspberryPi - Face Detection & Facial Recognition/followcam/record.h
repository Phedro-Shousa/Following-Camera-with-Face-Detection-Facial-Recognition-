#ifndef RECORD_H
#define RECORD_H

#include <QDialog>
#include "mainwindow.h"
#include "followcam.h"
namespace Ui {
class record;
}

class MainWindow;

class followcam;

class record : public QDialog
{
    Q_OBJECT

public:
    explicit record(QWidget *parent);
    ~record();
    void maina(MainWindow * main, followcam * orig_followcam );

public slots:

    void update_screen();

private:
    Ui::record *ui;
    MainWindow * m_mainwindow;
    followcam * m_followcam;
};

#endif // RECORD_H
