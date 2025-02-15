#ifndef MACROCONFIGVIEW_H
#define MACROCONFIGVIEW_H

#include <QWidget>
#include "loginet.h"

namespace Ui {
class MacroConfigView;
}

class MacroConfigView : public QWidget
{
    Q_OBJECT

public:

    explicit MacroConfigView(QWidget *parent = nullptr);
    ~MacroConfigView();

    QString macro_ghub;
    QString macroConfig_ghub;

    LogiNet *logiNet;

    void saveLua();
    void loadJson();
    void saveJson();

private slots:
    void on_pushButton_downloadmacro_clicked();

    void on_pushButton_apply_clicked();

    void on_checkBox_loginet_clicked(bool checked);

private:
    Ui::MacroConfigView *ui;
};

#endif // MACROCONFIGVIEW_H
