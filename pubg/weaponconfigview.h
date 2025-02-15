#ifndef WEAPONCONFIGVIEW_H
#define WEAPONCONFIGVIEW_H

#include <QWidget>
#include <QList>
#include "weaponconfig.h"

namespace Ui {
class WeaponConfigView;
}

class WeaponConfigView : public QWidget
{
    Q_OBJECT

public:
    int weapon_index = 1;
    int scope_index = 0;
    int attach1_index = 0;
    int attach2_index = 0;
    int attach4_index = 0;
    int pose_index = 0;
    explicit WeaponConfigView(QWidget *parent = nullptr);
    ~WeaponConfigView();

    QList<WeaponConfig> weaponConfigs;

    bool init_success = false;

    void setInfoByIndex(int index);
    void saveInfoByIndex(int index);

    void saveWeaponLua(void);

private slots:
    void on_comboBox_weapon_currentIndexChanged(int index);

    void on_comboBox_scope_currentIndexChanged(int index);

    void on_comboBox_attach1_currentIndexChanged(int index);

    void on_comboBox_attach2_currentIndexChanged(int index);

    void on_comboBox_attach4_currentIndexChanged(int index);

    void on_comboBox_pose_currentIndexChanged(int index);

    void on_pushButton_delete_clicked();

    void on_pushButton_save_clicked();

private:
    Ui::WeaponConfigView *ui;
};

#endif // WEAPONCONFIGVIEW_H
