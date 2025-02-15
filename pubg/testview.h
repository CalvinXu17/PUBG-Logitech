#ifndef TESTVIEW_H
#define TESTVIEW_H

#include <QWidget>

namespace Ui {
class testview;
}

class TestView : public QWidget
{
    Q_OBJECT

public:
    explicit TestView(QWidget *parent = nullptr);
    ~TestView();

private slots:
    void on_pushButton_clicked();

private:
    Ui::testview *ui;
};

#endif // TESTVIEW_H
