#ifndef RECOGNIZEVIEW_H
#define RECOGNIZEVIEW_H

#include <QWidget>
#include "recognizer.h"

namespace Ui {
class RecognizeView;
}

class RecognizeView : public QWidget
{
    Q_OBJECT

public:
    Recognizer *recog = NULL;
    BloodRecognize *bloodrecog = NULL;

    explicit RecognizeView(QWidget *parent = nullptr);
    ~RecognizeView();

    void loadJson();
    void saveJson();

private slots:
    void on_checkBox_enable_clicked(bool checked);

    void on_fpsSlider_valueChanged(int value);

    void updateView(const QImage &w1, int w1scope, int w1a1, int w1a2, int w1a4,
                    const QImage &w2, int w2scope, int w2a1, int w2a2, int w2a4,
                    int pos, int currentw);

    void on_checkBox_top_clicked(bool checked);

    void on_comboBox_adsmode_currentIndexChanged(int index);

    void on_checkBox_blood_clicked(bool checked);

    void on_pushButton_clicked();

    void on_comboBox_capturemode_currentIndexChanged(int index);

private:
    Ui::RecognizeView *ui;
};

#endif // RECOGNIZEVIEW_H
