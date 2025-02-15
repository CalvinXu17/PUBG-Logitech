#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static MainWindow *win;
    static int version;

    QSystemTrayIcon *tray;
    QMenu *traymenu;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

private slots:
    void onSystemTrayIconClicked(QSystemTrayIcon::ActivationReason reason);
    void updateSizes(int index);
};
#endif // MAINWINDOW_H
