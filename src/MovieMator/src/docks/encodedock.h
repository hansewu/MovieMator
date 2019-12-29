/*
 * Copyright (c) 2012-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ENCODEDOCK_H
#define ENCODEDOCK_H

#include <QDockWidget>
#include <QDomElement>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <MltProperties.h>
#include "advanceddock.h"
#include <QButtonGroup>


class QTreeWidgetItem;
class QStringList;
namespace Ui {
    class EncodeDock;
}
class AbstractJob;
class MeltJob;
class QTemporaryFile;
namespace Mlt {
    class Service;
}

class PresetsProxyModel : public QSortFilterProxyModel
{
protected:
    // 是否有 source_row和 source_parent所指定的项
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

class EncodeDock : public QDockWidget
{
    Q_OBJECT

public:
    // 构造函数
    explicit EncodeDock(QWidget *parent = nullptr);
    // 析构函数
    ~EncodeDock();



    // 通过文件 target创建 MeltJob并加入到 JOBS队列中
    void enqueueMelt(const QString& target, int realtime);


signals:
    // 捕获状态变化信号
    void captureStateChanged(bool);

    // 单击导出按钮信号
    void encodeButtonClicked();

public slots:
    // Producer被打开
    //void onProducerOpened();
    // 配置 Profile发生变化
    void onProfileChanged();
    void onUpdateAdvancedSetting(QString strFps);


private slots:
    // presetsTree单击的槽函数
    void on_presetsTree_clicked(const QModelIndex &index);
    // presetsTree激活的槽函数
    void on_presetsTree_activated(const QModelIndex &index);

    // encodeButton单击的槽函数
    void on_encodeButton_clicked();
    // streamButton单击的槽函数
   // void on_streamButton_clicked();

    // addPresetButton单击的槽函数
    void on_addPresetButton_clicked();

    // removePresetButton单击的槽函数
    void on_removePresetButton_clicked();

    // 执行完 m_immediateJob后调用
    // 清除 m_immediateJob并重置界面部分按钮内容
    void onFinished(AbstractJob*, bool isSuccess);

    // stopCaptureButton单击的槽函数
    //void on_stopCaptureButton_clicked();

    // vidoeRateControlCombo激活的槽函数
    //void on_videoRateControlCombo_activated(int index);

    // audioRateControlCombo激活的槽函数
   // void on_audioRateControlCombo_activated(int index);


    // presetsSearch文本内容发生变化的槽函数
    //void on_presetsSearch_textChanged(const QString &search);



    // 打开捕获的（导出的）文件
    void openCaptureFile();

    // formatCombo当前 index发生变化的槽函数
   // void on_formatCombo_currentIndexChanged(int index);

    // 视频时长发生变化的槽函数
    void on_videoBufferDurationChanged();

    // gopSpinner值发生变化的槽函数
    // GOP：图像组（帧数）
    //void on_gopSpinner_valueChanged(int value);

    //void on_widthSpinner_editingFinished();

   // void on_heightSpinner_editingFinished();

    void on_advancedButton_clicked();
    //接收传递过来的数据的槽
   // void receiveData(QString data);

    void selectPreset(QString presetname);

    void on_presetsList_clicked(const QModelIndex &index);

    void on_changeButtonGroup(int index);

    void on_visibilityChanged(bool bVisble);

    void onAddCustomPreset(QString newPreset); //xjp 2019.12.13 添加自定义preset到列表
    void onResetCurrentPreset();


private:

    // 界面 ui
    Ui::EncodeDock *ui;
    AdvancedDock* m_advanceddock;

    // 预置属性
    Mlt::Properties *m_presets;
    // 立即执行的任务？？？
    MeltJob* m_immediateJob;
    // 预置类型的扩展名
    QString m_extension;
    // 配置
    Mlt::Properties *m_profiles;
    // 预置文件的 model
    PresetsProxyModel m_presetsModel;
    // 导出的文件名
    QString m_outputFilename;
    // 是否是默认设置
    //bool m_isDefaultSettings;
    // 当前的预置文件属性
    Mlt::Properties *m_currentPreset;

    // 左边的预置树加载所有的预置列表
    void loadPresets();
    // 读取界面配置，设定 real_time为 realtime来创建 Mlt::Properties
    Mlt::Properties* collectProperties(int realtime);
    // 从界面配置给 node设定个属性值
    void collectProperties(QDomElement& node, int realtime);
    // 创建 MeltJob
    MeltJob* createMeltJob(Mlt::Service* service, const QString& target, int realtime, int pass = 0);
    // 执行 m_immediateJob
    void runMelt(const QString& target, int realtime = -1);
    // 添加水印
    void addWatermark(Mlt::Service* pService, QTemporaryFile& tmpProjectXml);

    // ???
    void encode(const QString& target);
    // 重置界面及预置为默认值
    void resetOptions();
    // 获取时间线上的 producer（如果 fromComboBox不为空的话）
    // 从timelineDock获取 Producer
    Mlt::Service* fromProducer() const;
    // 设置 preset为当前的预置
    void setCurrentPreset(Mlt::Properties *preset);

    // 获取当前预置在左边预置树中的父项
    QStandardItem *getTreeParentItem(const QString& text);

    QStandardItemModel m_modelList[7];// m_customModel;


    int m_currentSelectedClass;//当前选中的格式分类， 0-Custom, 1-video, 2-audio, 3-devices,4-tv, 5-lossless, 6-web
    QButtonGroup* m_formatButtonGroup;



    // 从预置的 Mlt::Properties中加载预设的配置
    // 从左边的预置树表里读取配置
    void loadPresetFromProperties(Mlt::Properties&);

};

#endif // ENCODEDOCK_H
