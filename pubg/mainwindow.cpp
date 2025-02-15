#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dxgicapture.h"

MainWindow* MainWindow::win = NULL;
int MainWindow::version = 1; // 版本

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(this->ui->tabWidget);
    this->updateSizes(0);

    tray = new QSystemTrayIcon(this);
    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onSystemTrayIconClicked(QSystemTrayIcon::ActivationReason)));
    connect(this->ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(updateSizes(int)));
    tray->setIcon(QIcon(":/pubg.ico"));
    tray->show();
    win = this;
}

MainWindow::~MainWindow()
{
    delete ui;
    delete tray;
}

void MainWindow::onSystemTrayIconClicked(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason) {
    //单击
    case QSystemTrayIcon::Trigger:
    //双击
    case QSystemTrayIcon::DoubleClick:
        if(this->isHidden()) {
            //恢复窗口显示
            this->show();
            //一下两句缺一均不能有效将窗口置顶
            this->setWindowState(Qt::WindowActive);
            this->activateWindow();

        }
        else {
            this->hide();
        }
        break;
    default:
        break;
    }
}

void MainWindow::changeEvent(QEvent *e)
{
    if((e->type()==QEvent::WindowStateChange)&&this->isMinimized()) {
        this->hide();
    } else QMainWindow::changeEvent(e);
}

void MainWindow::updateSizes(int index)
{
    for(int i=0;i<ui->tabWidget->count();i++)
    {
        if(i!=index)
        {
            ui->tabWidget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        } else {

        }
    }
    ui->tabWidget->widget(index)->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    ui->tabWidget->widget(index)->resize(ui->tabWidget->widget(index)->minimumSizeHint());
    ui->tabWidget->widget(index)->adjustSize();
}
