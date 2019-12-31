#ifndef ADVANCEDDOCK_H
#define ADVANCEDDOCK_H

#include <QDockWidget>
#include <MltProperties.h>

namespace Ui {
class AdvancedDock;
}
namespace Mlt {
    class Service;
}
class AdvancedDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit AdvancedDock(QWidget *parent = nullptr);
    ~AdvancedDock();
signals:
    // 捕获状态变化信号
   // void captureStateChanged(bool);
    void updateAdvancedSetting(QString strFps);

    //用来传递参数
   // void sendData(QString);

    void addCustomPreset(QString presetName);
    void resetCurrentPreset();

public slots:
    // 配置 Profile发生变化
   // void onProfileChanged();

    void setPreset (Mlt::Properties *preset, bool bVideo, bool bDisableReset);

private slots:


    // gopSpinner值发生变化的槽函数
    // GOP：图像组（帧数）
    void on_gopSpinner_valueChanged(int value);

    void on_widthSpinner_editingFinished();

    void on_heightSpinner_editingFinished();

    // vidoeRateControlCombo激活的槽函数
    void on_videoRateControlCombo_activated(int index);

    // audioRateControlCombo激活的槽函数
    void on_audioRateControlCombo_activated(int index);

    // scanModeCombo当前 index发生变化的槽函数
    void on_scanModeCombo_currentIndexChanged(int index);

    void on_resolutionBox_currentIndexChanged(const QString &arg1);

    // cancelButton单击的槽函数
    void on_cancelButton_clicked();

    void on_okButton_clicked();

    void on_addPresetButton_clicked();

    void on_resetPresetButton_clicked();

private:
    // 码率控制
    enum {
        // 平衡
        RateControlAverage = 0,
        // 稳定？？？
        RateControlConstant,
        // 性能
        RateControlQuality
    };
    Ui::AdvancedDock *ui;
    // 重置界面及预置为默认值
//    void resetOptions();

    // 是否是默认设置
    bool m_isDefaultSettings;

    // 预置属性
   // Mlt::Properties *m_presets;

    // 当前的预置文件属性
    Mlt::Properties *m_currentPreset;

    void updateCurrentPreset(int realtime);
    // 读取界面配置，设定 real_time为 realtime来创建 Mlt::Properties
    Mlt::Properties* collectProperties(int realtime);
};

#endif // ADVANCEDDOCK_H
