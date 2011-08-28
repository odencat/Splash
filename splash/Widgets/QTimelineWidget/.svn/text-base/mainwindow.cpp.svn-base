#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTimelinePanel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTimelinePanel* pQTimelinePanel = new QTimelinePanel(this);
    ui->mainContainer->addWidget(pQTimelinePanel);
}

MainWindow::~MainWindow()
{
    delete ui;
}
