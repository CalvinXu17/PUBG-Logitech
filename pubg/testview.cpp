#include "testview.h"
#include "ui_testview.h"
#include "logitech_driver.h"

TestView::TestView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::testview)
{
    ui->setupUi(this);
    mouse_open();
}

TestView::~TestView()
{
    mouse_close();
    delete ui;
}

void TestView::on_pushButton_clicked()
{
    moveR(0, 1);
}

