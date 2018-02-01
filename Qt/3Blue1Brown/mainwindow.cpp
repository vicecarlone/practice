#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->Slider,SIGNAL(valueChanged(int)),ui->widget,SLOT(setnumlight(int)));
}

MainWindow::~MainWindow()
{
    delete ui;

}

