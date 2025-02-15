#include "recognizeview.h"
#include "ui_recognizeview.h"
#include "cvutils.h"
#include "mainwindow.h"
#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QMessageBox>
#include "utils.h"
#include "bloodrecognize.h"
#include "dxgicapture.h"

RecognizeView::RecognizeView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecognizeView)
{
    ui->setupUi(this);
    for (int i = 0; i < this->ui->horizontalLayout_blood->count(); ++i) {
        QWidgetItem *item = (QWidgetItem*)(this->ui->horizontalLayout_blood->itemAt(i));
        if (item) {
            QWidget *widget = item->widget();
            if (widget) {
                widget->hide();
            }
        }
    }

    QDoubleValidator *dvalid = new QDoubleValidator(0.0, 100.0, 10, this);
    this->ui->lineEdit_xrate->setValidator(dvalid);
    this->ui->lineEdit_yrate->setValidator(dvalid);

    this->recog = Recognizer::getInstance();

//    cv::Mat m = cv::imread("F:\\python\\pubg\\error.png");
//    cv::cvtColor(m, m, cv::COLOR_BGR2BGRA);
//    qDebug() << "choose " << this->recog->weaponChoose(m);
//    m = CVUtils::cropMat(m, 1360, 94, 81, 31;)
//    CVUtils::to_black_white_above200(m ,false);
//    cv::Mat gun = cv::imread("F:\\python\\pubg\\gunname\\m416.png");
//    QString h1 = CVUtils::pHash(m);
//    QString h2 = CVUtils::pHash(gun);
//    qDebug() << CVUtils::HammingDistance(h1, h2);
//    recog->attachRecognize(m);
//    recog->printLua();

    this->loadJson();

    if(!this->recog->init_success) {
        this->ui->checkBox_enable->setChecked(false);
        this->ui->checkBox_enable->setDisabled(true);
        this->ui->checkBox_enable->setText("初始化失败");
        this->ui->fpsSlider->setDisabled(true);
    } else {
        connect(this->recog,
                SIGNAL(updateView(const QImage &, int, int, int, int,
                                  const QImage &, int, int, int, int,
                                  int, int)),
                this,
                SLOT(updateView(const QImage &, int, int, int, int,
                                const QImage &, int, int, int, int,
                                int, int)),
                Qt::QueuedConnection);
        this->recog->saveLua();
    }

}

RecognizeView::~RecognizeView()
{
    this->saveJson();
    delete ui;
    this->recog->stop();
    // delete this->recog;
}

void RecognizeView::loadJson()
{
    double xrate = 1.0;
    double yrate = 1.0;
    int color = 0;

    QString pwd = QApplication::applicationDirPath();
    QString resolution_file_path =  "/resource/recognizer.json";
    QFile jsonf(pwd + resolution_file_path);
    if(jsonf.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString jsonstr = jsonf.readAll();
        jsonf.close();
        QJsonParseError err;
        QJsonDocument document = QJsonDocument::fromJson(jsonstr.toUtf8(), &err);
        if(err.error == QJsonParseError::NoError) {
            QJsonObject jsonObj = document.object();

            if(jsonObj.contains("xrate")) {
                QJsonValue v = jsonObj.value("xrate");
                if(v.isDouble()) {
                    xrate = v.toDouble(1.0);
                }
            }
            if(jsonObj.contains("yrate")) {
                QJsonValue v = jsonObj.value("yrate");
                if(v.isDouble())
                {
                    yrate = v.toDouble(0.0);
                }
            }

            if(jsonObj.contains("bloodcolor")) {
                QJsonValue v = jsonObj.value("bloodcolor");
                color = v.toInt(0);
            }

            if(jsonObj.contains("scopemode")) {
                QJsonValue v = jsonObj.value("scopemode");
                this->ui->comboBox_adsmode->setCurrentIndex(v.toInt(0));
            }

            if(jsonObj.contains("rate")) {
                QJsonValue v = jsonObj.value("rate");
                int rate = v.toInt(16);
                this->ui->fpsSlider->setValue(rate);
                this->ui->label_fps->setText(QString::number(rate)+"fps");
            }

            if(jsonObj.contains("capturemode")) {
                QJsonValue v = jsonObj.value("capturemode");
                this->ui->comboBox_capturemode->setCurrentIndex(v.toInt(0));
            }
        }

    }

    this->ui->lineEdit_xrate->setText(QString::number(xrate));
    this->ui->lineEdit_yrate->setText(QString::number(yrate));
    this->ui->comboBox_bloodcolor->setCurrentIndex(color);

    if(this->recog->init_success) {
        this->recog->bloodrecog->setBloodColor(color);
        this->recog->bloodrecog->setRate(xrate, yrate);
    }
}

void RecognizeView::saveJson()
{
    QJsonObject jobj;

    jobj.insert("xrate", QJsonValue(this->ui->lineEdit_xrate->text().toDouble()));
    jobj.insert("yrate", QJsonValue(this->ui->lineEdit_yrate->text().toDouble()));
    jobj.insert("bloodcolor", QJsonValue(this->ui->comboBox_bloodcolor->currentIndex()));
    jobj.insert("scopemode", QJsonValue(this->ui->comboBox_adsmode->currentIndex()));
    jobj.insert("rate", QJsonValue(this->ui->fpsSlider->value()));
    jobj.insert("capturemode", QJsonValue(this->ui->comboBox_capturemode->currentIndex()));
    QJsonDocument doc;
    doc.setObject(jobj);

    QFile jsonf(QApplication::applicationDirPath() + "/resource/recognizer.json");
    if(jsonf.open(QIODevice::WriteOnly | QIODevice::Text)) {
        jsonf.write(doc.toJson());
        jsonf.close();
    }
}

void RecognizeView::on_checkBox_enable_clicked(bool checked)
{
    if(!checked) {
        this->ui->fpsSlider->setDisabled(true);
        if(this->recog && this->recog->init_success) {
            this->recog->stop();
        }
    } else {
        this->ui->fpsSlider->setDisabled(false);
        if(this->recog && this->recog->init_success) {
            this->recog->start();
        }
    }
}


void RecognizeView::on_fpsSlider_valueChanged(int value)
{
    this->ui->label_fps->setText(QString::number(value)+"fps");
    if(this->recog && this->recog->init_success) {
        this->recog->setFPS(value);
    }
}

void RecognizeView::updateView(const QImage &w1, int w1scope, int w1a1, int w1a2, int w1a4,
                               const QImage &w2, int w2scope, int w2a1, int w2a2, int w2a4,
                               int pos, int currentw)
{
    this->ui->w1->setPixmap(QPixmap::fromImage(w1));
    switch(w1scope) {
        case 0:
            this->ui->w1scope->setText("无");
            break;
        case 1:
            this->ui->w1scope->setText("红点");
            break;
        case 2:
            this->ui->w1scope->setText("全息");
            break;
        case 3:
            this->ui->w1scope->setText("二倍镜");
            break;
        case 4:
            this->ui->w1scope->setText("三倍镜");
            break;
        case 5:
            this->ui->w1scope->setText("四倍镜");
            break;
        case 6:
            this->ui->w1scope->setText("六倍镜");
            break;
    }

    switch (w1a1) {
        case 0:
            this->ui->w1a1->setText("无");
            break;
        case 1:
            this->ui->w1a1->setText("补偿器");
            break;
        case 2:
            this->ui->w1a1->setText("消焰器");
            break;
        case 3:
            this->ui->w1a1->setText("消音器");
            break;
        case 4:
            this->ui->w1a1->setText("制退器");
            break;
    }
    switch (w1a2) {
        case 0:
            this->ui->w1a2->setText("无");
            break;
        case 1:
            this->ui->w1a2->setText("垂直");
            break;
        case 2:
            this->ui->w1a2->setText("三角");
            break;
        case 3:
            this->ui->w1a2->setText("轻型");
            break;
        case 4:
            this->ui->w1a2->setText("激光");
            break;
        case 5:
            this->ui->w1a2->setText("拇指");
            break;
        case 6:
            this->ui->w1a2->setText("半截");
            break;
    }
    switch (w1a4) {
        case 0:
            this->ui->w1a4->setText("无");
            break;
        case 1:
            this->ui->w1a4->setText("枪托");
            break;
        case 2:
            this->ui->w1a4->setText("重型枪托");
            break;
    }


    this->ui->w2->setPixmap(QPixmap::fromImage(w2));
    switch(w2scope) {
        case 0:
            this->ui->w2scope->setText("无");
            break;
        case 1:
            this->ui->w2scope->setText("红点");
            break;
        case 2:
            this->ui->w2scope->setText("全息");
            break;
        case 3:
            this->ui->w2scope->setText("二倍镜");
            break;
        case 4:
            this->ui->w2scope->setText("三倍镜");
            break;
        case 5:
            this->ui->w2scope->setText("四倍镜");
            break;
        case 6:
            this->ui->w2scope->setText("六倍镜");
            break;
    }

    switch (w2a1) {
        case 0:
            this->ui->w2a1->setText("无");
            break;
        case 1:
            this->ui->w2a1->setText("补偿器");
            break;
        case 2:
            this->ui->w2a1->setText("消焰器");
            break;
        case 3:
            this->ui->w2a1->setText("消音器");
            break;
        case 4:
            this->ui->w2a1->setText("制退器");
            break;
    }
    switch (w2a2) {
        case 0:
            this->ui->w2a2->setText("无");
            break;
        case 1:
            this->ui->w2a2->setText("垂直");
            break;
        case 2:
            this->ui->w2a2->setText("三角");
            break;
        case 3:
            this->ui->w2a2->setText("轻型");
            break;
        case 4:
            this->ui->w2a2->setText("激光");
            break;
        case 5:
            this->ui->w2a2->setText("拇指");
            break;
        case 6:
            this->ui->w2a2->setText("半截");
            break;
    }
    switch (w2a4) {
        case 0:
            this->ui->w2a4->setText("无");
            break;
        case 1:
            this->ui->w2a4->setText("枪托");
            break;
        case 2:
            this->ui->w2a4->setText("重型枪托");
            break;
    }


    switch (pos) {
        case 0:
            this->ui->pos->setText("未知");
            break;
        case 1:
            this->ui->pos->setText("蹲");
            break;
        case 2:
            this->ui->pos->setText("站");
            break;
        case 3:
            this->ui->pos->setText("趴");
            break;
    }

    this->ui->currentw->setText(QString::number(currentw) + "号");
}


void RecognizeView::on_checkBox_top_clicked(bool checked)
{
    if(MainWindow::win) {
        if(checked) {
            MainWindow::win->setWindowFlag(Qt::WindowStaysOnTopHint, true);
            MainWindow::win->show();
        } else {
            MainWindow::win->setWindowFlag(Qt::WindowStaysOnTopHint, false);
            MainWindow::win->show();
        }
    }
}


void RecognizeView::on_comboBox_adsmode_currentIndexChanged(int)
{
    if(this->ui->comboBox_adsmode->currentText()=="HOLD") {
        Recognizer::getInstance()->setScopeMode(0);
    } else
        Recognizer::getInstance()->setScopeMode(1);
}


void RecognizeView::on_checkBox_blood_clicked(bool checked)
{
    this->recog->blood_enable = checked;
}


void RecognizeView::on_pushButton_clicked()
{
    float xrate = this->ui->lineEdit_xrate->text().toFloat();
    float yrate = this->ui->lineEdit_yrate->text().toFloat();
    int color = this->ui->comboBox_bloodcolor->currentIndex();
    this->recog->bloodrecog->setBloodColor(color);
    this->recog->bloodrecog->setRate(xrate, yrate);
}


void RecognizeView::on_comboBox_capturemode_currentIndexChanged(int index)
{
    CVUtils::screenshot_mode = index;
    if (index == 0) {
        if(!DXGICapture::getInstance()->success && !DXGICapture::getInstance()->init()) {
            QMessageBox msg;
            msg.setText(QString("DXGI初始化失败！\n" + DXGICapture::getInstance()->result));
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
        }
    } else {
        DXGICapture::getInstance()->deinit();
    }
    if(!index) {
        this->ui->fpsSlider->setRange(1, 60);
    } else {
        this->ui->fpsSlider->setRange(1, 30);
    }
}
