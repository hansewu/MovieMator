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

#include "encodedock.h"
#include "ui_encodedock.h"
#include "dialogs/addencodepresetdialog.h"
#include "jobqueue.h"
#include "mltcontroller.h"
#include "mainwindow.h"
#include "settings.h"
#include <qmlapplication.h>
#include "jobs/encodejob.h"
#include "shotcut_mlt_properties.h"
#include "registrationchecker.h"
#include "jobs/encodetask.h"
#include "encodetaskqueue.h"
#include "advanceddock.h"


#include <Logger.h>
#include <QtWidgets>
#include <QtXml>
#include <QtMath>
#include <QTimer>
#include <iostream>
#include "presettabstyle.h"


// formulas to map absolute value ranges to percentages as int
#define TO_ABSOLUTE(min, max, rel) qRound(float(min) + float((max) - (min) + 1) * float(rel) / 100.0f)
#define TO_RELATIVE(min, max, abs) qRound(100.0f * float((abs) - (min)) / float((max) - (min) + 1))

static double getBufferSize(Mlt::Properties& preset, const char* property);

EncodeDock::EncodeDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::EncodeDock),
    m_presets(Mlt::Repository::presets()),//预置属性
    m_immediateJob(nullptr),//立即执行任务
    m_profiles(Mlt::Profile::list()),// 配置
    //m_isDefaultSettings(true),// 是否是默认设置
    m_currentPreset(nullptr) // 当前的预置文件属性
{
    LOG_DEBUG() << "begin";
    ui->setupUi(this);

    toggleViewAction()->setIcon(windowIcon());//设置图标
    //ui->videoCodecThreadsSpinner->setMaximum(QThread::idealThreadCount());
    //if (QThread::idealThreadCount() < 3)
        //ui->parallelCheckbox->setHidden(true);

    //connect(ui->videoBitrateCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(on_videoBufferDurationChanged()));
    //connect(ui->videoBufferSizeSpinner, SIGNAL(valueChanged(double)), this, SLOT(on_videoBufferDurationChanged()));

    m_presetsModel.setSourceModel(new QStandardItemModel(this));// 预置文件的 model
    m_presetsModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
//    ui->presetsTree->setModel(&m_presetsModel);
//    ui->presetsTree->setAlternatingRowColors(false);
//    ui->presetsTree->setStyleSheet("QTreeView {selection-background-color: #97ffff}");

    for(int index=0; index<7; index++)
    {
        m_modelList[index].clear();
    //    m_modelList[index]->clear();
    }

    loadPresets();

    // populate the combos
    Mlt::Consumer c(MLT.profile(), "avformat");
    //Q_ASSERT(c.is_valid());
    if (c.is_valid())
    {
    c.set("f", "list");
    c.set("acodec", "list");
    c.set("vcodec", "list");
    c.start();
    c.stop();




    }



    //remove 'Other' tab
    // ui->tabWidget->removeTab(3);
    //ui->parallelCheckbox->setHidden(true);
    ui->widget_bottom_toolbar->setStyleSheet(".QWidget {background-color: black; background-image: url(:/export-window-bg.png); background-repeat: repeat-x; border: 0px}");
    m_advanceddock=new AdvancedDock();
    m_advanceddock->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    m_advanceddock->setWindowModality(Qt::WindowModal);
    //m_advanceddock->onProfileChanged();



    connect(m_advanceddock,SIGNAL(updateAdvancedSetting(QString)), this, SLOT(onUpdateAdvancedSetting(QString)));
    connect(m_advanceddock,SIGNAL(addCustomPreset(QString)),this,SLOT(onAddCustomPreset(QString)));
    connect(m_advanceddock,SIGNAL(resetCurrentPreset()),this, SLOT(onResetCurrentPreset()));



    //xjp 格式按钮设置了checkable属性后，设置checked状态和非checked状态的颜色
    ui->customFormat->setStyleSheet("QPushButton{background-color: rgb(40, 46, 52);\
                                                 border:1px solid #aaaaaa;} \
                                     QPushButton:checked{background-color: rgb(161, 67, 42); border:1px solid #aaaaaa;}");
    ui->videoFormat->setStyleSheet("QPushButton{background-color: rgb(40, 46, 52);\
                                   border:1px solid #aaaaaa;} \
                       QPushButton:checked{background-color: rgb(161, 67, 42); border:1px solid #aaaaaa;}");
    ui->audioFormat->setStyleSheet("QPushButton{background-color: rgb(40, 46, 52);\
                                   border:1px solid #aaaaaa;} \
                       QPushButton:checked{background-color: rgb(161, 67, 42); border:1px solid #aaaaaa;}");
    ui->deviceFormat->setStyleSheet("QPushButton{background-color: rgb(40, 46, 52);\
                                    border:1px solid #aaaaaa;} \
                        QPushButton:checked{background-color: rgb(161, 67, 42); border:1px solid #aaaaaa;}");
    ui->losslessFormat->setStyleSheet("QPushButton{background-color: rgb(40, 46, 52);\
                                      border:1px solid #aaaaaa;} \
                          QPushButton:checked{background-color: rgb(161, 67, 42); border:1px solid #aaaaaa;}");
    ui->webFormat->setStyleSheet("QPushButton{background-color: rgb(40, 46, 52);\
                                 border:1px solid #aaaaaa;} \
                     QPushButton:checked{background-color: rgb(161, 67, 42); border:1px solid #aaaaaa;}");
    ui->tvFormat->setStyleSheet("QPushButton{background-color: rgb(40, 46, 52);\
                                border:1px solid #aaaaaa;} \
                    QPushButton:checked{background-color: rgb(161, 67, 42); border:1px solid #aaaaaa;}");

    //xjp 用QButtonGrop控件实现几个format按钮之间的选中状态互斥
    m_formatButtonGroup = new QButtonGroup;
    m_formatButtonGroup->addButton(ui->customFormat,0);
    m_formatButtonGroup->addButton(ui->videoFormat,1);
    m_formatButtonGroup->addButton(ui->audioFormat,2);
    m_formatButtonGroup->addButton(ui->deviceFormat,3);
    m_formatButtonGroup->addButton(ui->tvFormat,4);
    m_formatButtonGroup->addButton(ui->losslessFormat,5);
    m_formatButtonGroup->addButton(ui->webFormat,6);


    connect(m_formatButtonGroup,SIGNAL(buttonClicked(int)),this,SLOT(on_changeButtonGroup(int)));

    //将第一次显示的格式设为video分类下的第一个MP4(h264)
    m_currentSelectedClass = 1;
    ui->videoFormat->setChecked(true);
    ui->presetsList->setModel(&m_modelList[1]);

    QModelIndex indexFromList = m_modelList[1].index(0,0);
    ui->presetsList->setCurrentIndex(indexFromList);
    ui->presetsList->setFocus();


    LOG_DEBUG() << "end";


    ui->presetsList->setStyleSheet("QListView{background: rgb(26,30,34)}\
                                   QListView::item{height: 40px}\
                                   QScrollBar:vertical{background:rgba(6,30,34);border:0px; width:8px;}\
                                   ScrollBar::handle:vertical{background-color:#bbbbbb;border-radius:7px;}");


  //  completer_->popup()->setStyleSheet("QListView{background: rgb(35,35,35);color:white; font-size: 12pt; font-family: Calibri Light; font-weight:normal}"

   connect(this,SIGNAL(visibilityChanged(bool)),this,SLOT(on_visibilityChanged(bool)));

    m_currentPreset = NULL;
    m_bFistShown = true;


}

EncodeDock::~EncodeDock()
{
    delete ui;
    delete m_presets;
    delete m_profiles;
    if (m_currentPreset)
        delete m_currentPreset;
    delete m_advanceddock;
}


void EncodeDock::loadPresetFromProperties(Mlt::Properties& preset)
{
    QStringList other;

    m_extension.clear();

    QString strPresetName = preset.get("meta.preset.name");  //m_modelList[m_currentSelectedClass].data(0, Qt::UserRole + 1).toString()；
    if(strPresetName == "DVD (dv_pal)")
    {
        preset.set("width","720");
        preset.set("height","576");
        preset.set("aspect",double(4.0/3.0));
        preset.set("r","25");

    }
    else if(strPresetName == "DVD (dv_ntsc)")
    {
        preset.set("width","720");
        preset.set("height","480");
        preset.set("aspect",double(4.0/3.0));
        preset.set("r","29.97");
    }


    for (int i = 0; i < preset.count(); i++)
    {
        QString name(preset.get_name(i));
         if (name == "meta.preset.extension")
         {
             m_extension = preset.get("meta.preset.extension");
         }

         else if (name == "width" )
         {
            QString str = QString("%1 %2 %3 ")
                   .arg(preset.get("width")).arg("x").arg(preset.get("height"));
            ui->resolutionValue->setText(str);
         }
         else if (name == "ar" && strlen(preset.get("ar")))
         {
            ui->sampleRateValue->setText(QString("%1Hz").arg(preset.get("ar")));
         }
         else if(name == "ab" && strlen(preset.get("ab")))
         {
            ui->bitrateValue->setText(QString("%1bps").arg(preset.get("ab")));
         }


    }


    if(preset.get_double("aspect") == 0) //xjp，当preset文件中没有aspect参数时，从profile中获取默认值
    {
         preset.set("aspect", double(MLT.profile().display_aspect_num())/double(MLT.profile().display_aspect_den()) );

    }
}

QStandardItem* EncodeDock::getTreeParentItem(const QString &text)
{
    int i = 0;
    QStandardItem *parentItem = nullptr;
    QStandardItemModel* sourceModel = qobject_cast<QStandardItemModel*>(m_presetsModel.sourceModel());//(QStandardItemModel*) m_presetsModel.sourceModel();

    int rowCount = sourceModel->invisibleRootItem()->rowCount();
    for (i = 0; i < rowCount; i++)
    {
        parentItem = sourceModel->invisibleRootItem()->child(i);
        QString itemText = parentItem->text();
        if (itemText.compare(text) == 0)
            return parentItem;
    }
    return nullptr;
}

void EncodeDock::loadPresets()
{
    Q_ASSERT(m_presets);


    QStandardItemModel* sourceModel = qobject_cast<QStandardItemModel*>(m_presetsModel.sourceModel());   //(QStandardItemModel*) m_presetsModel.sourceModel()
    Q_ASSERT(sourceModel);
    sourceModel->clear();

    QStandardItem* parentItem = new QStandardItem(tr("Custom"));
    Q_ASSERT(parentItem);
    parentItem->setData(0, Qt::UserRole + 2);
    sourceModel->invisibleRootItem()->appendRow(parentItem);//追加行
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
    if (dir.cd("presets") && dir.cd("encode")) {
        QStringList entries = dir.entryList(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
        foreach (QString name, entries) {
            QStandardItem* item = new QStandardItem(name);
            Q_ASSERT(item);
            item->setData(name);

            item->setData(0, Qt::UserRole + 2);
     //       parentItem->appendRow(item);

            m_modelList[0].appendRow(item);
         //   m_modelList[0].setItem(0,item);
        }
    }

//    parentItem = new QStandardItem(tr("Stock"));
//    sourceModel->invisibleRootItem()->appendRow(parentItem);
    QString prefix("consumer/avformat/");
    if (m_presets && m_presets->is_valid()) {
        for (int j = 0; j < m_presets->count(); j++) {
            QString name(m_presets->get_name(j));
            if (name.startsWith(prefix)) {
                Mlt::Properties preset(static_cast<mlt_properties>(m_presets->get_data(name.toLatin1().constData())));  // (mlt_properties) m_presets->get_data(name.toLatin1().constData())
                if (preset.get_int("meta.preset.hidden"))
                    continue;

                int categoryIndex = preset.get_int("meta.preset.category");
                int indexInCategory = preset.get_int("meta.preset.index");
//                if (preset.get("meta.preset.name"))
//                    name = QString::fromUtf8(preset.get("meta.preset.name"));
//                else
//                {

                    // use relative path and filename
                    name.remove(0, prefix.length());
                    QStringList textParts = name.split('/');
                    if (textParts.count() > 1) {
                        // if the path is a profile name, then change it to "preset (profile)"
                        QString category = textParts.at(0);
                        parentItem = this->getTreeParentItem(category);


                        if (parentItem == nullptr) //如果没有这个分类，就新建一个
                        {
                            parentItem = new QStandardItem(category);
                            sourceModel->invisibleRootItem()->appendRow(parentItem);
                            parentItem->setData(categoryIndex, Qt::UserRole + 2);
                        }

                        textParts.removeFirst();//移除分类部分
                        if (textParts.count() > 1)
                        {
                            QString profile = textParts.at(0);
                            textParts.removeFirst();
                            if (m_profiles->get_data(profile.toLatin1().constData()))
                                name = QString("%1 (%2)").arg(textParts.join("/")).arg(profile);
                        }
                        else {
                           name.remove(0, category.length() + 1);
                        }
                    }
//                }

                    if (preset.get("meta.preset.name"))
                        name = QString::fromUtf8(preset.get("meta.preset.name"));
                    QStandardItem* item = new QStandardItem(name);
                    Q_ASSERT(item);


                    item->setData(QString(m_presets->get_name(j)));
                    item->setData(indexInCategory, Qt::UserRole + 2);
                    if (preset.get("meta.preset.note"))
                        item->setToolTip(QString("<p>%1</p>").arg(QString::fromUtf8(preset.get("meta.preset.note"))));
                //    parentItem->appendRow(item);
                    m_modelList[categoryIndex].setItem(indexInCategory,item);

            }
        }
    }



    m_presetsModel.setSortRole(Qt::UserRole + 2);
    m_presetsModel.sort(0);

//    ui->presetsTree->expandAll();
   // selectPreset("MP4 (H264)");



}

Mlt::Properties* EncodeDock::collectProperties(int realtime)
{
     Mlt::Properties* p = new Mlt::Properties;

     if (p && p->is_valid())
     {
         if (m_currentPreset && m_currentPreset->is_valid()) {
             for (int i = 0; i < m_currentPreset->count(); i++)
                 if (m_currentPreset->get_name(i) && strcmp(m_currentPreset->get_name(i), ""))
                     p->set(m_currentPreset->get_name(i), m_currentPreset->get(i));
         }
     }

     return p;
//    if (p && p->is_valid())
//    {
//        foreach (QString line, ui->advancedTextEdit->toPlainText().split("\n"))
//            p->parse(line.toUtf8().constData());
//        if (realtime)
//            p->set("real_time", realtime);
//        p->set("meta.preset.name", ui->presetLabel->text().toLatin1().constData());
//        if (ui->formatCombo->currentIndex() != 0)
//            p->set("f", ui->formatCombo->currentText().toLatin1().constData());
//        if (ui->disableAudioCheckbox->isChecked())
//        {
//            p->set("an", 1);
//            p->set("audio_off", 1);
//        }
//        else
//        {
//            const QString& acodec = ui->audioCodecCombo->currentText();
//            if (ui->audioCodecCombo->currentIndex() > 0)
//                p->set("acodec", ui->audioCodecCombo->currentText().toLatin1().constData());
//            p->set("ar", ui->sampleRateCombo->currentText().toLatin1().constData());
//            if (ui->audioRateControlCombo->currentIndex() == RateControlAverage
//                    || ui->audioRateControlCombo->currentIndex() == RateControlConstant)
//            {
//                p->set("ab", ui->audioBitrateCombo->currentText().toLatin1().constData());
//                if (acodec == "libopus")
//                {
//                    if (RateControlConstant == ui->audioRateControlCombo->currentIndex())
//                        p->set("vbr", "off");
//                    else
//                        p->set("vbr", "constrained");
//                }
//            }
//            else if (acodec == "libopus")
//            {
//                p->set("vbr", "on");
//                p->set("ab", ui->audioBitrateCombo->currentText().toLatin1().constData());
//            }
//            else
//            {
//                int aq = ui->audioQualitySpinner->value();
//                if (acodec == "libmp3lame")
//                    aq = TO_ABSOLUTE(9, 0, aq);
//                else if (acodec == "libvorbis" || acodec == "vorbis")
//                    aq = TO_ABSOLUTE(0, 10, aq);
//                else
//                    aq = TO_ABSOLUTE(0, 500, aq);
//                p->set("aq", aq);
//            }
//        }
//        if (ui->disableVideoCheckbox->isChecked())
//        {
//            p->set("vn", 1);
//            p->set("video_off", 1);
//        }
//        else
//        {
//            const QString& vcodec = ui->videoCodecCombo->currentText();
//            if (ui->videoCodecCombo->currentIndex() > 0)
//                p->set("vcodec", vcodec.toLatin1().constData());
//            if (vcodec == "libx265")
//            {
//                // Most x265 parameters must be supplied through x265-params.
//                QString x265params = QString::fromUtf8(p->get("x265-params"));
//                switch (ui->videoRateControlCombo->currentIndex())
//                {
//                    case RateControlAverage:
//                        p->set("vb", ui->videoBitrateCombo->currentText().toLatin1().constData());
//                        break;
//                    case RateControlConstant:
//                    {
//                        QString b = ui->videoBitrateCombo->currentText();
//                         // x265 does not expect bitrate suffixes and requires Kb/s
//                        b.replace('k', "").replace('M', "000");
//                        x265params = QString("bitrate=%1:vbv-bufsize=%2:vbv-maxrate=%3:%4")
//                        .arg(b).arg(int(ui->videoBufferSizeSpinner->value() * 8)).arg(b).arg(x265params);
//                        break;
//                    }
//                    case RateControlQuality:
//                    {
//                        int vq = ui->videoQualitySpinner->value();
//                        x265params = QString("crf=%1:%2").arg(TO_ABSOLUTE(51, 0, vq)).arg(x265params);
//                    // Also set crf property so that custom presets can be interpreted properly.
//                        p->set("crf", TO_ABSOLUTE(51, 0, vq));
//                        break;
//                    }
//                }
//                x265params = QString("keyint=%1:bframes=%2:%3").arg(ui->gopSpinner->value())
//                            .arg(ui->bFramesSpinner->value()).arg(x265params);
//                p->set("x265-params", x265params.toUtf8().constData());
//            }
//            else
//            {
//                switch (ui->videoRateControlCombo->currentIndex())
//                {
//                    case RateControlAverage:
//                         p->set("vb", ui->videoBitrateCombo->currentText().toLatin1().constData());
//                         break;
//                    case RateControlConstant:
//                    {
//                        const QString& b = ui->videoBitrateCombo->currentText();
//                        p->set("vb", b.toLatin1().constData());
//                        p->set("vminrate", b.toLatin1().constData());
//                        p->set("vmaxrate", b.toLatin1().constData());
//                        p->set("vbufsize", int(ui->videoBufferSizeSpinner->value() * 8 * 1024));
//                        break;
//                    }
//                    case RateControlQuality:
//                    {
//                        int vq = ui->videoQualitySpinner->value();
//                        if (vcodec == "libx264")
//                        {
//                            p->set("crf", TO_ABSOLUTE(51, 0, vq));
//                        }
//                        else if (vcodec.startsWith("libvpx"))
//                        {
//                            p->set("crf", TO_ABSOLUTE(63, 0, vq));
//                            p->set("vb", 0); // VP9 needs this to prevent constrained quality mode.
//                        }
//                        else
//                        {
//                            p->set("qscale", TO_ABSOLUTE(31, 1, vq));
//                        }
//                        break;
//                    }
//                }
//                p->set("g", ui->gopSpinner->value());
//                p->set("bf", ui->bFramesSpinner->value());
//            }
//            p->set("width", ui->widthSpinner->value());
//            p->set("height", ui->heightSpinner->value());
//            p->set("aspect", double(ui->aspectNumSpinner->value()) / double(ui->aspectDenSpinner->value()));
//            p->set("progressive", ui->scanModeCombo->currentIndex());
//            p->set("top_field_first", ui->fieldOrderCombo->currentIndex());
//            switch (ui->deinterlacerCombo->currentIndex())
//            {
//                case 0:
//                    p->set("deinterlace_method", "onefield");
//                    break;
//                case 1:
//                    p->set("deinterlace_method", "linearblend");
//                    break;
//                case 2:
//                    p->set("deinterlace_method", "yadif-nospatial");
//                    break;
//                default:
//                    p->set("deinterlace_method", "yadif");
//                    break;
//            }
//            switch (ui->interpolationCombo->currentIndex())
//            {
//                case 0:
//                    p->set("rescale", "nearest");
//                    break;
//                case 1:
//                    p->set("rescale", "bilinear");
//                    break;
//                case 2:
//                    p->set("rescale", "bicubic");
//                    break;
//                default:
//                    p->set("rescale", "hyper");
//                    break;
//            }
//            if (qFloor(ui->fpsSpinner->value() * 10.0) == 239)
//            {
//                p->set("frame_rate_num", 24000);
//                p->set("frame_rate_den", 1001);
//            }
//            else if (qFloor(ui->fpsSpinner->value() * 10.0) == 299)
//            {
//                p->set("frame_rate_num", 30000);
//                p->set("frame_rate_den", 1001);
//            }
//            else if (qFloor(ui->fpsSpinner->value() * 10.0) == 479)
//            {
//                p->set("frame_rate_num", 48000);
//                p->set("frame_rate_den", 1001);
//            }
//            else if (qFloor(ui->fpsSpinner->value() * 10.0) == 599)
//            {
//                p->set("frame_rate_num", 60000);
//                p->set("frame_rate_den", 1001);
//            }
//            else
//                p->set("r", qFloor(ui->fpsSpinner->value()));

//            if (ui->videoCodecCombo->currentText() == "prores" || ui->formatCombo->currentText() == "image2")
//                p->set("threads", 1);
//            else if (ui->videoCodecThreadsSpinner->value() == 0
//                     && ui->videoCodecCombo->currentText() != "libx264"
//                     && ui->videoCodecCombo->currentText() != "libx265")
//                p->set("threads", QThread::idealThreadCount() - 1);
//            else
//                p->set("threads", ui->videoCodecThreadsSpinner->value());
//            if (ui->dualPassCheckbox->isEnabled() && ui->dualPassCheckbox->isChecked())
//                p->set("pass", 1);
//        }

//        return p;
//    }

}


void EncodeDock::collectProperties(QDomElement& node, int realtime)
{
   // Mlt::Properties* p = collectProperties(realtime);
    if (m_currentPreset && m_currentPreset->is_valid()) {
        for (int i = 0; i < m_currentPreset->count(); i++)
            if (m_currentPreset->get_name(i) && strcmp(m_currentPreset->get_name(i), "") && strcmp(m_currentPreset->get_name(i), "r"))
                node.setAttribute(m_currentPreset->get_name(i), m_currentPreset->get(i));
    }
    //delete p;
}


typedef struct {
    int nFrameRateNum;
    int nFrameRateDen;
} FRAME_RATE;

static int timeStringToFrames(const char *strTime, FRAME_RATE framerate)
{
    Q_ASSERT(strTime);
    Q_ASSERT(framerate.nFrameRateDen > 0 && framerate.nFrameRateNum > 0);

    Mlt::Properties mltProperties;
    Mlt::Profile    mltProfile;

    mltProfile.set_frame_rate(framerate.nFrameRateNum, framerate.nFrameRateDen);
    mltProperties.set("_profile", mltProfile.get_profile(), 0);

    return mltProperties.time_to_frames(strTime);
}


static int convertTimeToFramesWithNewFps(QString strOldTime, FRAME_RATE framerateOld, FRAME_RATE framerateNew)
{
    //clock格式（hh:mm:ss.sss）不转换
//    if ( (strOldTime.contains(":") && strOldTime.contains("."))
//         || (strOldTime.contains(":") && strOldTime.contains(","))
//         )
//        return -1;

    int nFramesNew = 0;
    int nFrameOld = timeStringToFrames(strOldTime.toUtf8().constData(), framerateOld);

    double dFpsOld = framerateOld.nFrameRateNum * 1.0 / framerateOld.nFrameRateDen;
    double dFpsNew = framerateNew.nFrameRateNum * 1.0 / framerateNew.nFrameRateDen;

    double dFramesNew =  nFrameOld * dFpsNew / dFpsOld;

    srand(time(NULL));

    if (qFuzzyIsNull(dFramesNew - floor(dFramesNew)))
        nFramesNew = floor(dFramesNew);
    else if(rand()*1.0/RAND_MAX > dFramesNew - floor(dFramesNew))
        nFramesNew = floor(dFramesNew);
    else
        nFramesNew = floor(dFramesNew) + 1;

    return nFramesNew;
}

static void recaculateTimeAttribute(QDomElement &domElement, FRAME_RATE framerateOld, FRAME_RATE framerateNew)
{
    Q_ASSERT(!domElement.isNull());

    //处理dom in out属性
    QDomAttr domAttrOut = domElement.attributeNode("out");
    if (!domAttrOut.isNull())
    {
        int nFrameIn = 0;
        int nFrameOut = 0;
        int nFrameLength = 0;

        QString strOldTimeOut = domAttrOut.value();
        nFrameOut = timeStringToFrames(strOldTimeOut.toUtf8().constData(), framerateOld);

        QDomAttr domAttrIn = domElement.attributeNode("in");
        if (!domAttrIn.isNull())
        {
            QString strOldTimeIn = domAttrIn.value();
            nFrameIn = timeStringToFrames(strOldTimeIn.toUtf8().constData(), framerateOld);
        }

        if (nFrameOut > 0)
            nFrameLength = nFrameOut - nFrameIn + 1;

        int nFrameLengthNew = convertTimeToFramesWithNewFps(QString::number(nFrameLength), framerateOld, framerateNew);
        int nFrameInNew = convertTimeToFramesWithNewFps(QString::number(nFrameIn), framerateOld, framerateNew);
        int nFrameOutNew = nFrameInNew + nFrameLengthNew - 1;

        domAttrOut.setValue(QString::number(nFrameOutNew));
        if (!domAttrIn.isNull())
            domAttrIn.setValue(QString::number(nFrameInNew));
    }

    //处理length属性
    QDomAttr domAttrLength = domElement.attributeNode("length");
    if (!domAttrLength.isNull())
    {
        QString strOldTimeLength = domAttrLength.value();

        int nFramesLengthNew = convertTimeToFramesWithNewFps(strOldTimeLength, framerateOld, framerateNew);
        if (nFramesLengthNew >= 0 )
            domAttrLength.setValue(QString::number(nFramesLengthNew));
    }
}


static int recalculateTimeInDomElement(QDomElement domElement, FRAME_RATE framerateOld, FRAME_RATE framerateNew)
{
    Q_ASSERT(!domElement.isNull());
    Q_ASSERT(framerateOld.nFrameRateDen > 0 && framerateOld.nFrameRateNum > 0);
    Q_ASSERT(framerateNew.nFrameRateDen > 0 && framerateNew.nFrameRateNum > 0);

    int nErrorCode = 1;

    if ((framerateOld.nFrameRateDen == framerateNew.nFrameRateDen)
            && (framerateOld.nFrameRateNum == framerateNew.nFrameRateNum))
        return nErrorCode;

    if (framerateOld.nFrameRateDen <= 0 || framerateOld.nFrameRateNum <= 0)
        return nErrorCode;

    if (framerateNew.nFrameRateDen <= 0 || framerateNew.nFrameRateNum <= 0)
        return nErrorCode;


    //process attributes of element
    recaculateTimeAttribute(domElement, framerateOld, framerateNew);


    //process dom text of element
    QString strElementName = domElement.attribute("name");
    if (strElementName.compare("length", Qt::CaseInsensitive) == 0)
    {
        QDomNode   domNodeText = domElement.firstChild();
        QString    strOldTime  = domNodeText.nodeValue();

        int nFramesNew = convertTimeToFramesWithNewFps(strOldTime, framerateOld, framerateNew);
        if (nFramesNew > 0)
            domNodeText.setNodeValue(QString::number(nFramesNew));
    }

    //process child elements
    QDomNodeList nodeListChildNode = domElement.childNodes();
    for (int i = 0; i < nodeListChildNode.count(); i++)
    {
        QDomElement childElement = nodeListChildNode.at(i).toElement();
        if (!childElement.isNull())
            recalculateTimeInDomElement(childElement, framerateOld, framerateNew);
    }
    return 0;
}


MeltJob* EncodeDock::createMeltJob(Mlt::Service* service, const QString& target, int realtime, int pass)
{
    // if image sequence, change filename to include number
    QString mytarget = target;
//    if (!ui->disableVideoCheckbox->isChecked()) {
//        const QString& codec = ui->videoCodecCombo->currentText();
//        if (codec == "bmp" || codec == "dpx" || codec == "png" || codec == "ppm" ||
//                codec == "targa" || codec == "tiff" || (codec == "mjpeg" )) //&& ui->formatCombo->currentText() == "image2"
//        {
//            QFileInfo fi(mytarget);
//            mytarget = QString("%1/%2-%05d.%3").arg(fi.path()).arg(fi.baseName()).arg(fi.completeSuffix());
//        }
//    }

    // get temp filename
    QTemporaryFile tmp;

    tmp.open();
    tmp.close();

    MLT.saveXML(tmp.fileName(), service, false /* without relative paths */);
    //添加水印
    addWatermark(service, tmp);

    // parse xml
    QFile f1(tmp.fileName());
    f1.open(QIODevice::ReadOnly);
    QDomDocument dom(tmp.fileName());
    dom.setContent(&f1);
    f1.close();


    // add consumer element
    QDomElement consumerNode = dom.createElement("consumer");
    QDomNodeList profiles = dom.elementsByTagName("profile");
    if (profiles.isEmpty())
        dom.documentElement().insertAfter(consumerNode, dom.documentElement());
    else
        dom.documentElement().insertAfter(consumerNode, profiles.at(profiles.length() - 1));
    consumerNode.setAttribute("mlt_service", "avformat");
    consumerNode.setAttribute("target", mytarget);
    collectProperties(consumerNode, realtime);
   /* if ("libx265"== ui->videoCodecCombo->currentText()) //== ui->videoCodecCombo->currentText())
    {
        if (pass == 1 || pass == 2) {
            QString x265params = consumerNode.attribute("x265-params");
            x265params = QString("pass=%1:stats=%2:%3")
                .arg(pass).arg(mytarget.replace(":", "\\:") + "_2pass.log").arg(x265params);
            consumerNode.setAttribute("x265-params", x265params);
        }
    } else*/ {
        if (pass == 1 || pass == 2) {
            consumerNode.setAttribute("pass", pass);
            consumerNode.setAttribute("passlogfile", mytarget + "_2pass.log");
        } if (pass == 1) {
            consumerNode.setAttribute("fastfirstpass", 1);
            consumerNode.removeAttribute("acodec");
            consumerNode.setAttribute("an", 1);
        } else {
            consumerNode.removeAttribute("fastfirstpass");
        }
    }
//    if (ui->formatCombo->currentIndex() == 0 &&
//            ui->audioCodecCombo->currentIndex() == 0 &&
//            (mytarget.endsWith(".mp4") || mytarget.endsWith(".mov")))
//        consumerNode.setAttribute("strict", "experimental");

    // Add autoclose to playlists.
    QDomNodeList playlists = dom.elementsByTagName("playlist");
    for (int i = 0; i < playlists.length();++i)
        playlists.item(i).toElement().setAttribute("autoclose", 1);

    if(m_currentPreset->get_int("vn") == 0)//if(!ui->disableVideoCheckbox->isChecked())
    {
        FRAME_RATE  framerateOld;
        FRAME_RATE  framerateNew;

        QDomElement domElementProfile;
        if (!profiles.isEmpty())
        {
            domElementProfile = profiles.at(profiles.length() - 1).toElement();
            framerateOld.nFrameRateNum = domElementProfile.attribute("frame_rate_num").toInt();
            framerateOld.nFrameRateDen = domElementProfile.attribute("frame_rate_den").toInt();
        }
        else
        {
            Q_ASSERT(false);
        }

        if (!consumerNode.attribute("frame_rate_num").isEmpty() && !consumerNode.attribute("frame_rate_den").isEmpty())
        {
            framerateNew.nFrameRateNum = consumerNode.attribute("frame_rate_num").toInt();
            framerateNew.nFrameRateDen = consumerNode.attribute("frame_rate_den").toInt();
        }
        else if (!consumerNode.attribute("r").isEmpty())
        {
            framerateNew.nFrameRateNum = consumerNode.attribute("r").toInt();
            framerateNew.nFrameRateDen = 1;
        }
        else
        {
            Q_ASSERT(false);
        }


        //根据输出帧率转换xml中保存的帧数
        int nRet = recalculateTimeInDomElement(dom.documentElement(), framerateOld, framerateNew);
        if (nRet == 0)
        {
            if (!domElementProfile.isNull())
            {
                domElementProfile.setAttribute("frame_rate_num", framerateNew.nFrameRateNum);
                domElementProfile.setAttribute("frame_rate_den", framerateNew.nFrameRateDen);
            }
        }
        qDebug()<<domElementProfile.attribute("frame_rate_num");
        qDebug()<<domElementProfile.attribute("frame_rate_den");
    }



    return new EncodeJob(target, dom.toString(2));
}

//添加水印
void EncodeDock::addWatermark(Mlt::Service* pService, QTemporaryFile& tmpProjectXml)
{
    Q_ASSERT(pService);
    if (!pService)
    {
        return;
    }

    //添加水印滤镜（网站使用版、网站免费版，app store免费版）
#if defined(SHARE_VERSION) || defined(MOVIEMATOR_FREE)
    Mlt::Filter *pSoftwareNameTextFilter = nullptr;
    Mlt::Filter *pHomePagetextFilter     = nullptr;

    if (Registration.registrationType() != Registration_None
            || Settings.isSubscribed())
        return;

    //if (Registration.registrationType() == Registration_None)
    {
        //显示软件名
        QString strSoftwareName             = tr("MovieMator");
        std::string strSoftwareNameTemp     = strSoftwareName.toStdString();
        pSoftwareNameTextFilter             = new Mlt::Filter(MLT.profile(), "dynamictext");

        Q_ASSERT(pSoftwareNameTextFilter);
        pSoftwareNameTextFilter->set("argument", strSoftwareNameTemp.c_str());
#ifdef Q_OS_WIN
        pSoftwareNameTextFilter->set("family", "微软雅黑");
#endif
        pSoftwareNameTextFilter->set("size", 207);
        pSoftwareNameTextFilter->set("weight", 750);
        pSoftwareNameTextFilter->set("letter_spaceing", 0);
        pSoftwareNameTextFilter->set("style", "normal");
        pSoftwareNameTextFilter->set("shear_x", 0);
        pSoftwareNameTextFilter->set("shadow_distance", 0);
        pSoftwareNameTextFilter->set("shadow_angle", 44);
        pSoftwareNameTextFilter->set("fgcolour", 255, 255, 255, 255);
        pSoftwareNameTextFilter->set("olcolour", 255, 187, 66, 48);
        pSoftwareNameTextFilter->set("pad", 0);
        pSoftwareNameTextFilter->set("halign", "center");
        pSoftwareNameTextFilter->set("valign", "middle");
        pSoftwareNameTextFilter->set("outline", 3);
        pSoftwareNameTextFilter->set("trans_fix_rotate_x", 0);
        pSoftwareNameTextFilter->set("trans_scale_x", 1);
        pSoftwareNameTextFilter->set("trans_ox", 0);
        pSoftwareNameTextFilter->set("trans_oy", 0);
        pSoftwareNameTextFilter->set("trans_scale_aspect_ratio", 1);

        //显示主页网址
        QString strHomePage     = tr("http://moviemator.net");
        pHomePagetextFilter     = new Mlt::Filter(MLT.profile(), "dynamictext");

        Q_ASSERT(pHomePagetextFilter);
        pHomePagetextFilter->set("argument", strHomePage.toLatin1().data());
#ifdef Q_OS_WIN
        pHomePagetextFilter->set("family", "微软雅黑");
#endif
        pHomePagetextFilter->set("size", 89);
        pHomePagetextFilter->set("weight", 750);
        pHomePagetextFilter->set("letter_spaceing", 0);
        pHomePagetextFilter->set("style", "normal");
        pHomePagetextFilter->set("shear_x", -0.2);
        pHomePagetextFilter->set("shadow_distance", 0);
        pHomePagetextFilter->set("shadow_angle", 44);
        pHomePagetextFilter->set("fgcolour", 255, 255, 255, 255);
        pHomePagetextFilter->set("olcolour", 255, 187, 66, 48);
        pHomePagetextFilter->set("pad", 0);
        pHomePagetextFilter->set("halign", "center");
        pHomePagetextFilter->set("valign", "middle");
        pHomePagetextFilter->set("outline", 2);
        pHomePagetextFilter->set("trans_fix_rotate_x", 0);
        pHomePagetextFilter->set("trans_scale_x", 1);
        pHomePagetextFilter->set("trans_ox", 0);
        pHomePagetextFilter->set("trans_oy", 0);
        pHomePagetextFilter->set("trans_scale_aspect_ratio", 1);

        pSoftwareNameTextFilter->set("geometry", 0.684375, 0, 0.328125, 0.2875);
        pSoftwareNameTextFilter->set("bgcolour", 0, 128, 128, 128);
        pSoftwareNameTextFilter->set("transparent_alpha", 1);

        pHomePagetextFilter->set("geometry", 0.698438, 0.1725, 0.299219, 0.123611);
        pHomePagetextFilter->set("bgcolour", 0, 128, 128, 128);
        pHomePagetextFilter->set("transparent_alpha", 1);

#if MOVIEMATOR_FREE && SHARE_VERSION //国区免费版
        pSoftwareNameTextFilter->set("geometry", 0.854375, 0, 0.13125, 0.115);
        pSoftwareNameTextFilter->set("bgcolour", 0, 0, 255, 255);
        pSoftwareNameTextFilter->set("transparent_alpha", 0.25);

        pHomePagetextFilter->set("geometry", 0.860438, 0.0825, 0.1196876, 0.0494444);
        pHomePagetextFilter->set("bgcolour", 0, 0, 255, 255);
        pHomePagetextFilter->set("transparent_alpha", 0.25);
#endif

        pService->attach(*pSoftwareNameTextFilter);
        pService->attach(*pHomePagetextFilter);
    }
#endif

    //保存已添加水印的工程为xml工程文件
    MLT.saveXML(tmpProjectXml.fileName(), pService, false /* without relative paths */);

    //从当前工程中移除水印
#if defined(SHARE_VERSION) || defined(MOVIEMATOR_FREE)
    if (Registration.registrationType() != Registration_None
            || Settings.isSubscribed())
        return;

        pService->detach(*pSoftwareNameTextFilter);
        pService->detach(*pHomePagetextFilter);

        delete pSoftwareNameTextFilter;
        pSoftwareNameTextFilter = nullptr;

        delete pHomePagetextFilter;
        pHomePagetextFilter = nullptr;
#endif
}

void EncodeDock::runMelt(const QString& target, int realtime)
{
    Mlt::Service* service = fromProducer();
    if (!service) {
        // For each playlist item.
        if (MAIN.playlist() && MAIN.playlist()->count() > 0) {
            // Use the first playlist item.
            QScopedPointer<Mlt::ClipInfo> info(MAIN.playlist()->clip_info(0));
            if (!info) return;
            QString xml = MLT.XML(info->producer);
            QScopedPointer<Mlt::Producer> producer(
                new Mlt::Producer(MLT.profile(), "xml-string", xml.toUtf8().constData()));
            Q_ASSERT(producer);
            producer->set_in_and_out(info->frame_in, info->frame_out);
            m_immediateJob = createMeltJob(producer.data(), target, realtime);
            Q_ASSERT(m_immediateJob);
            if (m_immediateJob) {
                m_immediateJob->setIsStreaming(true);
                connect(m_immediateJob, SIGNAL(finished(AbstractJob*,bool)), this, SLOT(onFinished(AbstractJob*,bool)));
                m_immediateJob->start();
            }
            return;
        } else {
            service = MLT.producer();
        }
    }
    m_immediateJob = createMeltJob(service, target, realtime);
    Q_ASSERT(m_immediateJob);
    if (m_immediateJob) {
        m_immediateJob->setIsStreaming(true);
        connect(m_immediateJob, SIGNAL(finished(AbstractJob*,bool)), this, SLOT(onFinished(AbstractJob*,bool)));
        m_immediateJob->start();
    }
}

void EncodeDock::enqueueMelt(const QString& target, int realtime)
{
    Mlt::Service* service = fromProducer();
    if (!service) {
        // For each playlist item.
        if (MAIN.playlist() && MAIN.playlist()->count() > 1) {
            QFileInfo fi(target);
            int n = MAIN.playlist()->count();
            int digits = QString::number(n + 1).size();
            for (int i = 0; i < n; i++) {
                QScopedPointer<Mlt::ClipInfo> info(MAIN.playlist()->clip_info(i));
                if (!info) continue;
                QString xml = MLT.XML(info->producer);
                QScopedPointer<Mlt::Producer> producer(
                    new Mlt::Producer(MLT.profile(), "xml-string", xml.toUtf8().constData()));
                Q_ASSERT(producer);
                producer->set_in_and_out(info->frame_in, info->frame_out);
                QString filename = QString("%1/%2-%3.%4").arg(fi.path()).arg(fi.baseName())
                                                         .arg(i + 1, digits, 10, QChar('0')).arg(fi.completeSuffix());
                int pass = 0;
                MeltJob* job = createMeltJob(producer.data(), filename, realtime, pass);
                if (job) {
                    JOBS.add(job);
                    if (pass) {
                        job = createMeltJob(producer.data(), filename, realtime, 2);
                        if (job)
                            JOBS.add(job);
                    }
                }
            }
        }
    }
    else {
        int pass =   0;
        MeltJob* job = createMeltJob(service, target, realtime, pass);
        if (job) {
            JOBS.add(job);
            if (pass) {
                job = createMeltJob(service, target, realtime, 2);
                if (job)
                    JOBS.add(job);
            }
        }
    }
}

void EncodeDock::encode(const QString& target)
{
    bool isMulti = true;
    Mlt::Producer* producer = new Mlt::Producer(MLT.producer());
    Q_ASSERT(producer);
    double volume = MLT.volume();
    MLT.closeConsumer();
    MLT.close();
    producer->seek(0);
    MLT.setProducer(producer, isMulti);
    MLT.consumer()->set("1", "avformat");
    MLT.consumer()->set("1.target", target.toUtf8().constData());
    Mlt::Properties* p = collectProperties(-1);
    qDebug()<<"******* xjp encode is called:";
    if (p && p->is_valid()) {
        for (int i = 0; i < p->count(); i++)
        {
            MLT.consumer()->set(QString("1.%1").arg(p->get_name(i)).toLatin1().constData(), p->get(i));
            qDebug()<<p->get(i);
            qDebug()<<p->get_name(i);
        }
    }
    delete p;
    if (target.endsWith(".mp4") || target.endsWith(".mov"))//)//ui->formatCombo->currentIndex() == 0 &&
        //ui->audioCodecCombo->currentIndex() == 0 &&
        MLT.consumer()->set("1.strict", "experimental");
    MLT.setVolume(volume);
    MLT.play();
}



static double getBufferSize(Mlt::Properties& preset, const char* property)
{
    Q_ASSERT(preset.is_valid());
    Q_ASSERT(property);
    double size = preset.get_double(property);
    const QString& s = preset.get(property);
    // evaluate suffix
    if (s.endsWith('k')) size *= 1000;
    if (s.endsWith('M')) size *= 1000000;
    // convert to KiB
    return double(qCeil(size / 1024 / 8 * 100)) / 100;
}

void EncodeDock::on_presetsTree_clicked(const QModelIndex &index)
{
    Q_ASSERT(m_profiles);

    if (!index.parent().isValid())
        return;
    QString name = m_presetsModel.data(index, Qt::UserRole + 1).toString();
    if (!name.isEmpty()) {
        Mlt::Properties* preset;
        if (m_presetsModel.data(index.parent()).toString() == tr("Custom")) {
            ui->removePresetButton->setEnabled(true);
            preset = new Mlt::Properties();
            Q_ASSERT(preset);
            QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
            if (dir.cd("presets") && dir.cd("encode"))
                preset->load(dir.absoluteFilePath(name).toLatin1().constData());

        }
        else {
            ui->removePresetButton->setEnabled(false);
            preset = new Mlt::Properties(static_cast<mlt_properties>(m_presets->get_data(name.toLatin1().constData())));    //(mlt_properties) m_presets->get_data(name.toLatin1().constData())
            Q_ASSERT(preset);
        }
        if (preset->is_valid()) {
            QStringList textParts = name.split('/');
            setCurrentPreset(preset);
            resetOptions();
            onProfileChanged();
            if (textParts.count() > 4) {
                // textParts = ['consumer', 'avformat', 'category', profile, preset].
                QString folder = textParts.at(3);
                if (m_profiles->get_data(folder.toLatin1().constData())) {
                    // only set these fields if the folder is a profile
                    Mlt::Profile p(folder.toLatin1().constData());
                    Q_ASSERT(p.is_valid());
//                    ui->widthSpinner->setValue(p.width());
//                    ui->heightSpinner->setValue(p.height());
//                    ui->aspectNumSpinner->setValue(p.display_aspect_num());
//                    ui->aspectDenSpinner->setValue(p.display_aspect_den());
//                    ui->scanModeCombo->setCurrentIndex(p.progressive());
                    //ui->frames->setValue(p.fps());

//                    ui->widthSpinner->setEnabled(false);
//                    ui->heightSpinner->setEnabled(false);
//                    ui->aspectNumSpinner->setEnabled(false);
//                    ui->aspectDenSpinner->setEnabled(false);
//                    ui->scanModeCombo->setEnabled(false);
                   // ui->frames->setEnabled(false);
                }
            }
            loadPresetFromProperties(*preset);

        }
        else {
            delete preset;
        }
    }
}

void EncodeDock::on_presetsTree_activated(const QModelIndex &index)
{
    on_presetsTree_clicked(index);
}

//FIXME: 要支持获取单个clip或者playlist，则需要修改
Mlt::Service *EncodeDock::fromProducer() const
{
    return MAIN.multitrack();
//    QString from = ui->fromCombo->currentData().toString();
//    if (from == "clip")
//        return MLT.isClip()? MLT.producer() : MLT.savedProducer();
//    else if (from == "playlist")
//        return MAIN.playlist();
//    else if (from == "timeline")
//        return MAIN.multitrack();
//    else
//        return 0;
//    QString from = ui->fromCombo->currentData().toString();
//    if (!from.isEmpty()) {
//        return MAIN.multitrack();
//    }

    return nullptr;
}

void EncodeDock::on_encodeButton_clicked()
{
    if (!MLT.producer())
        return;

    MLT.pause();

    //提示购买注册
#if SHARE_VERSION
#if MOVIEMATOR_PRO || MOVIEMATOR_FREE
    if (Registration.registrationType() == Registration_None)
    {
        this->hide();
        int resultCode = MAIN.showRegistrationTipDialog();
        if (resultCode == 100 || resultCode == 101)
            return;
    }
#endif
#endif

    //弹出订阅窗口
#ifndef SHARE_VERSION
#if MOVIEMATOR_FREE
    if (!Settings.isSubscribed())
    {
        MAIN.showInAppDialog();
    }
#endif
#endif


    if (ui->encodeButton->text() == tr("Stop Capture")) {
        MLT.closeConsumer();
        ui->encodeButton->setText(tr("Capture File"));
        emit captureStateChanged(false);
        QTimer::singleShot(1000, this, SLOT(openCaptureFile()));
        return;
    }
    bool seekable = MLT.isSeekable();
    QString directory = Settings.encodePath();
    if (!m_extension.isEmpty()) {
        directory += "/untitiled.";
        directory += m_extension;
    }
#ifdef Q_OS_MAC
    else {
        directory.append("/untitiled.mp4");
    }
#endif

    m_outputFilename = QFileDialog::getSaveFileName(this,
        seekable? tr("Export File") : tr("Capture File"), directory, tr("*.%1").arg(m_extension));
    if (!m_outputFilename.isEmpty()) {
        QFileInfo fi(m_outputFilename);
        MLT.pause();
        Settings.setEncodePath(fi.path());
        if (!m_extension.isEmpty()) {
            if (fi.suffix().isEmpty()) {
                m_outputFilename += '.';
                m_outputFilename += m_extension;
            }
        }
    // wzq 使用qmelt
      if (seekable) {
            // Batch encode
            JOBS.setShowFirst(true);
            int threadCount = QThread::idealThreadCount();
            if (threadCount > 2 )//&& ui->parallelCheckbox->isChecked()
                threadCount = qMin(threadCount - 1, 4);
            else
                threadCount = 1;
            enqueueMelt(m_outputFilename, Settings.playerGPU()? -1 : -threadCount);

            this->hide();

          //  MAIN.onJobsDockTriggered(true);
        }

    }
}

void EncodeDock::onProfileChanged()
{
    Q_ASSERT(MLT.profile().is_valid());
    int width = MLT.profile().width();
    int height = MLT.profile().height();
    double sar = MLT.profile().sar();



    int dar_numerator = width * int(sar);
    int dar_denominator = height;



    if (height > 0) {
        switch (int(sar * width / height * 100)) {
        case 133:
            dar_numerator = 4;
            dar_denominator = 3;
            break;
        case 177:
            dar_numerator = 16;
            dar_denominator = 9;
            break;
        case 56:
            dar_numerator = 9;
            dar_denominator = 16;
            break;
        }
    }



    if (m_currentPreset)
    {
        loadPresetFromProperties(*m_currentPreset);

        QString str = QString("%1 %2 %3 ")
                           .arg(width).arg("x").arg(height);
        ui->resolutionValue->setText(str);

        ui->fpsValue->setText(m_currentPreset->get("r"));


        QString strValue = m_currentPreset->get("ar");
        if(strValue.length())
            ui->sampleRateValue->setText(m_currentPreset->get("ar"));

        strValue = m_currentPreset->get("ab");
        if(strValue.length())
            ui->bitrateValue->setText(m_currentPreset->get("ab"));

    }

}

void EncodeDock::on_addPresetButton_clicked()
{
    Mlt::Properties* data = collectProperties(0);
    Q_ASSERT(data);
    AddEncodePresetDialog dialog(this);
    QStringList ls;

//    if (data && data->is_valid())
//        for (int i = 0; i < data->count(); i++)
//            if (strlen(data->get_name(i)) > 0)
//                ls << QString("%1=%2").arg(data->get_name(i)).arg(data->get(i));

    dialog.setWindowTitle(tr("Add Export Preset"));

//    dialog.setProperties(ls.join("\n"));
    //dialog.setPresetName(ui->presetLabel->text());
    if (dialog.exec() == QDialog::Accepted) {
        QString preset = dialog.presetName();
        data->set("meta.preset.name", preset.toLatin1().constData());
        if (data && data->is_valid())
            for (int i = 0; i < data->count(); i++)
                if (strlen(data->get_name(i)) > 0)
                    ls << QString("%1=%2").arg(data->get_name(i)).arg(data->get(i));
        dialog.setProperties(ls.join("\n"));
        QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
        QString subdir("encode");

        if (!preset.isEmpty()) {
            if (!dir.exists())
                dir.mkpath(dir.path());
            if (!dir.cd("presets")) {
                if (dir.mkdir("presets"))
                    dir.cd("presets");
            }
            if (!dir.cd(subdir)) {
                if (dir.mkdir(subdir))
                    dir.cd(subdir);
            }
            QFile f(dir.filePath(preset));
            if (f.open(QIODevice::WriteOnly | QIODevice::Text))
                f.write(dialog.properties().toUtf8());

            // add the preset and select it
            onAddCustomPreset(preset);   //   loadPresets();
//            QModelIndex parentIndex = m_presetsModel.index(0, 0);
//            int n = m_presetsModel.rowCount(parentIndex);
//            for (int i = 0; i < n; i++) {
//                QModelIndex index = m_presetsModel.index(i, 0, parentIndex);
//                if (m_presetsModel.data(index).toString() == preset) {
////                    ui->presetsTree->setCurrentIndex(index);
//                    break;
//                }
//            }
        }
    }
    delete data;
}

void EncodeDock::on_removePresetButton_clicked()
{
    QModelIndex index = ui->presetsList->currentIndex();
    QString preset = m_modelList[0].data(index).toString();
    QMessageBox dialog(QMessageBox::Question,
                       tr("Delete Preset"),
                       tr("Are you sure you want to delete %1?").arg(preset),
                       QMessageBox::No | QMessageBox::Yes,
                       this);

    dialog.setIconPixmap(QPixmap(":/icons/moviemator-pro-logo-64.png"));

#if defined(MOVIEMATOR_FREE) && !defined(SHARE_VERSION)
    dialog.setIconPixmap(QPixmap(":/icons/moviemator-logo-64.png"));
#endif

    dialog.setDefaultButton(QMessageBox::Yes);
    dialog.setEscapeButton(QMessageBox::No);
    dialog.setWindowModality(QmlApplication::dialogModality());
    int result = dialog.exec();
    if (result == QMessageBox::Yes) {
        QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
        if (dir.cd("presets") && dir.cd("encode")) {
            dir.remove(preset);
            m_modelList[0].removeRow(index.row());// //removeRow(index.row(), index.parent());
        }
    }
}

void EncodeDock::onFinished(AbstractJob* job, bool isSuccess)
{
    Q_ASSERT(job);
    Q_UNUSED(isSuccess)
    if (!MLT.isSeekable())
        ui->encodeButton->setText(tr("Capture File"));
    m_immediateJob = nullptr;
    delete job;
    emit captureStateChanged(false);
    ui->encodeButton->setDisabled(false);        
}


bool PresetsProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
    return !source_parent.isValid() || 
        sourceModel()->data(index).toString().contains(filterRegExp()) ||
        sourceModel()->data(index, Qt::ToolTipRole).toString().contains(filterRegExp());
}



void EncodeDock::openCaptureFile()
{
    MAIN.open(m_outputFilename);
}

void EncodeDock::on_videoBufferDurationChanged()
{
//    QString vb = ui->videoBitrateCombo->currentText();
//    vb.replace('k', "").replace('M', "000");
//    double duration = /*(double)*/ui->videoBufferSizeSpinner->value() * 8.0 / vb.toDouble();
//    QString label = QString(tr("KiB (%1s)")).arg(duration);
//    ui->videoBufferSizeSuffixLabel->setText(label);
}



void EncodeDock::setCurrentPreset(Mlt::Properties *preset)
{
    Q_ASSERT(preset);
    Q_ASSERT(preset);
    if (preset)
    {
         Mlt::Properties* p = new Mlt::Properties;
         if (p && p->is_valid())
         {
             if (preset && preset->is_valid()) {
                 for (int i = 0; i < preset->count(); i++)
                 {
                     if (preset->get_name(i) && strcmp(preset->get_name(i), ""))
                     {
                         p->set(preset->get_name(i), preset->get(i));
                     }
                 }
             }

             if (m_currentPreset)
             {
                 delete m_currentPreset;
                 m_currentPreset = p;
             }
             else
             {
                  m_currentPreset = p;
             }
         }
    }
}

void EncodeDock::on_advancedButton_clicked()
{
//    ui->presetsTree->setModel(NULL);
    bool bVideo = (m_currentSelectedClass!=2)?true:false;
    bool bDisableReset = (m_currentSelectedClass==0)?true:false;
    m_advanceddock->setPreset(m_currentPreset,bVideo,bDisableReset);
    m_advanceddock->show();
}


void EncodeDock::resetOptions()
{
    // Reset all controls to default values.
    //ui->formatCombo->setCurrentIndex(0);
     ui->resolutionLabel->setEnabled(true);
 //    ui->durationEdit->setEnabled(true);
//     ui->fpsCombox->setEnabled(true);
//    ui->widthSpinner->setEnabled(true);
//    ui->heightSpinner->setEnabled(true);
//    ui->aspectNumSpinner->setEnabled(true);
//    ui->aspectDenSpinner->setEnabled(true);
//    ui->scanModeCombo->setEnabled(true);
//    ui->fpsSpinner->setEnabled(true);
//    ui->deinterlacerCombo->setEnabled(true);
//    ui->deinterlacerCombo->setCurrentIndex(3);
//    ui->interpolationCombo->setEnabled(true);
//    ui->interpolationCombo->setCurrentIndex(1);

//    //ui->videoBitrateCombo->lineEdit()->setText("2M");
//    //ui->videoBufferSizeSpinner->setValue(224);
//    ui->gopSpinner->blockSignals(true);
//    ui->gopSpinner->setValue(13);
//    ui->gopSpinner->blockSignals(false);
//    ui->bFramesSpinner->setValue(2);
//    ui->videoCodecThreadsSpinner->setValue(0);
//    ui->dualPassCheckbox->setChecked(false);
//    ui->disableVideoCheckbox->setChecked(false);

//    ui->sampleRateCombo->lineEdit()->setText("44100");
//    ui->audioBitrateCombo->lineEdit()->setText("384k");
//    //ui->audioQualitySpinner->setValue(50);
//    ui->disableAudioCheckbox->setChecked(false);

   // on_videoBufferDurationChanged();

//    Mlt::Properties preset;
//    preset.set("f", "mp4");
//    preset.set("movflags", "+faststart");
//    preset.set("vcodec", "libx264");
//    preset.set("crf", "21");
//    preset.set("preset", "fast");
//    preset.set("acodec", "aac");
//    preset.set("meta.preset.extension", "mp4");


//     m_currentPreset->set("f", "mp4");
//     preset.set("movflags", "+faststart");
//     m_currentPreset->set("vcodec", "libx264");
     m_currentPreset->set("crf", "21");
     m_currentPreset->set("preset", "fast");

     if(m_currentSelectedClass != 0)
     {
         m_currentPreset->set("an", "0");
         m_currentPreset->set("vn", "0");
         m_currentPreset->set("frame_rate_num",24);
         m_currentPreset->set("frame_rate_den",1);
         QString fps = QString("%1").arg(MLT.profile().fps());
         m_currentPreset->set("r",fps.toLatin1().constData());

         m_currentPreset->set("width",MLT.profile().width());
         m_currentPreset->set("height",MLT.profile().height());
     }



//     m_currentPreset->set("acodec", "aac");
//     preset.set("meta.preset.extension", "mp4");

     ui->sampleRateValue->setText("44100Hz");
     ui->bitrateValue->setText("384Kbps");
    loadPresetFromProperties(*m_currentPreset);
}


void EncodeDock::selectPreset(QString presetname)
{
//    int CategoryCount = m_presetsModel.rowCount(parentIndex);
//    for (int k = 0; k < CategoryCount; k++)
//    {
        QModelIndex parentIndex = m_presetsModel.index(0, 0);
        int n = m_presetsModel.rowCount(parentIndex);
        for (int i = 0; i < n; i++)
        {
            QModelIndex index = m_presetsModel.index(i, 0, parentIndex);
            if (m_presetsModel.data(index).toString() == presetname)
            {
             //   ui->presetsTree->setCurrentIndex(index);
                break;
            }
        }
//     }

}



//void EncodeDock::on_customFormat_clicked()
//{
//    ui->presetsList->setModel(&m_modelList[0]);
//    m_currentSelectedClass = 0;

//}

//void EncodeDock::on_videoFormat_clicked()
//{
//    ui->presetsList->setModel(&m_modelList[1]);
//     m_currentSelectedClass = 1;
//}

//void EncodeDock::on_audioFormat_clicked()
//{
//    ui->presetsList->setModel(&m_modelList[2]);
//     m_currentSelectedClass = 2;
//}

//void EncodeDock::on_deviceFormat_clicked()
//{
//    ui->presetsList->setModel(&m_modelList[3]);
//     m_currentSelectedClass = 3;
//}

//void EncodeDock::on_tvFormat_clicked()
//{
//    ui->presetsList->setModel(&m_modelList[4]);
//     m_currentSelectedClass = 4;
//}

//void EncodeDock::on_losslessFormat_clicked()
//{
//    ui->presetsList->setModel(&m_modelList[5]);
//     m_currentSelectedClass = 5;
//}

//void EncodeDock::on_webFormat_clicked()
//{
//    ui->presetsList->setModel(&m_modelList[6]);
//     m_currentSelectedClass = 6;
//}

void EncodeDock::on_presetsList_clicked(const QModelIndex &index)
{
    Q_ASSERT(m_profiles);

    if (!index.isValid())
        return;


    m_currentFormatIndex = index;
    QString name = m_modelList[m_currentSelectedClass].data(index, Qt::UserRole + 1).toString();//m_presetsModel.data(index, Qt::UserRole + 1).toString();
    if (!name.isEmpty()) {
        Mlt::Properties* preset;
        if(m_currentSelectedClass == 0)
       // if (m_modelList[m_currentSelectedClass].data(index.parent()).toString() == tr("Custom"))
        {
            ui->removePresetButton->setEnabled(true);
            preset = new Mlt::Properties();
            Q_ASSERT(preset);
            QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
            if (dir.cd("presets") && dir.cd("encode"))
                preset->load(dir.absoluteFilePath(name).toLatin1().constData());

        }
        else {
            ui->removePresetButton->setEnabled(false);
            preset = new Mlt::Properties(static_cast<mlt_properties>(m_presets->get_data(name.toLatin1().constData())));    //(mlt_properties) m_presets->get_data(name.toLatin1().constData())

            Q_ASSERT(preset);
        }
        if (preset->is_valid()) {
            QStringList textParts = name.split('/');
            setCurrentPreset(preset);
            resetOptions();
            onProfileChanged();
            if (textParts.count() > 4) {
                // textParts = ['consumer', 'avformat', 'category', profile, preset].
                QString folder = textParts.at(3);
                if (m_profiles->get_data(folder.toLatin1().constData())) {
                    // only set these fields if the folder is a profile
                    Mlt::Profile p(folder.toLatin1().constData());
                    Q_ASSERT(p.is_valid());


                    //    ui->widthSpinner->setValue(p.width());
                    //                    ui->heightSpinner->setValue(p.height());
                    //                    ui->aspectNumSpinner->setValue(p.display_aspect_num());
                    //                    ui->aspectDenSpinner->setValue(p.display_aspect_den());
                    //                    ui->scanModeCombo->setCurrentIndex(p.progressive());
                                        //ui->frames->setValue(p.fps());

                    //                    ui->widthSpinner->setEnabled(false);
                    //                    ui->heightSpinner->setEnabled(false);
                    //                    ui->aspectNumSpinner->setEnabled(false);
                    //                    ui->aspectDenSpinner->setEnabled(false);
                    //                    ui->scanModeCombo->setEnabled(false);
                                       // ui->frames->setEnabled(false);

                }
            }
            loadPresetFromProperties(*preset);
        }
        else {
            delete preset;
        }
    }
}


 void EncodeDock::onUpdateAdvancedSetting(QString strFps)
 {

     if (m_currentPreset)
     {
         loadPresetFromProperties(*m_currentPreset);
         QString str = QString("%1 %2 %3 ")
                            .arg(m_currentPreset->get("width")).arg("x").arg(m_currentPreset->get("height"));
         ui->resolutionValue->setText(str);


//
         ui->fpsValue->setText(strFps);
         ui->presetsList->setFocus();

     }

 }


 void EncodeDock::onAddCustomPreset(QString newPreset)
 {
     Q_ASSERT(m_presets);


     QStandardItemModel* sourceModel = qobject_cast<QStandardItemModel*>(m_presetsModel.sourceModel());   //(QStandardItemModel*) m_presetsModel.sourceModel()
     Q_ASSERT(sourceModel);
     sourceModel->clear();

     QStandardItem* parentItem = new QStandardItem(tr("Custom"));
     Q_ASSERT(parentItem);
     parentItem->setData(0, Qt::UserRole + 2);
     sourceModel->invisibleRootItem()->appendRow(parentItem);//追加行
     QDir dir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
     if (dir.cd("presets") && dir.cd("encode"))
     {
          QStandardItem* item = new QStandardItem(newPreset);
          Q_ASSERT(item);
          item->setData(newPreset);

          item->setData(0, Qt::UserRole + 2);
      //       parentItem->appendRow(item);

        //  m_modelList[0].appendRow(item);
             m_modelList[0].insertRow(0,item);
      }

//      ui->presetsList->setModel(&m_modelList[0]);
//      ui->customFormat->click();


 }


 void EncodeDock::on_changeButtonGroup(int index)
 {
     Q_ASSERT(index >= 0 && index < 7);// xjp, 7表示格式分类按钮的个数，当
     QPushButton *checkedButton = qobject_cast<QPushButton*>(m_formatButtonGroup->checkedButton());
     QString checkedBtnObjectName = checkedButton->objectName();

     if(checkedButton)
     {
         ui->presetsList->setModel(&m_modelList[index]);
         m_currentSelectedClass = index;

         //切换时分类时，默认选中第一个格式 -- TODO xjp 2019.12.13 有选中index，但没有高亮显示index
         QModelIndex indexFromList = m_modelList[index].index(0,0);
         ui->presetsList->clicked(indexFromList);
         ui->presetsList->setCurrentIndex(indexFromList);
         ui->presetsList->setFocus();

         if(index == 2)
         {
             ui->bitrateLabel->setVisible(true);
             ui->sampleRateLabel->setVisible(true);
             ui->bitrateValue->setVisible(true);
             ui->sampleRateValue->setVisible(true);

             ui->resolutionLabel->setVisible(false);
             ui->resolutionValue->setVisible(false);
             ui->fpsLabel->setVisible(false);
             ui->fpsValue->setVisible(false);
         }
         else
         {
             ui->bitrateLabel->setVisible(false);
             ui->sampleRateLabel->setVisible(false);
             ui->bitrateValue->setVisible(false);
             ui->sampleRateValue->setVisible(false);

             ui->resolutionLabel->setVisible(true);
             ui->resolutionValue->setVisible(true);
             ui->fpsLabel->setVisible(true);
             ui->fpsValue->setVisible(true);
         }

     }

     //xjp 2019.12.20 每次切换格式，也设置高级设置窗口的控件的值，防止用户不修改advance窗口的设置，时，也能取到advanceddock的值
     bool bVideo = (m_currentSelectedClass!=2)?true:false;
     bool bDisableReset = (m_currentSelectedClass==0)?true:false;
     m_advanceddock->setPreset(m_currentPreset,bVideo,bDisableReset);


 }




void EncodeDock::on_visibilityChanged(bool bVisible)
{
    if(bVisible)
    {

        ui->durationValue->setText(MAIN.multitrack()->get_length_time());

       if(m_bFistShown)
       {
           ui->videoFormat->click();
           m_bFistShown = false;
       }
       else
       {

            if(m_currentSelectedClass == 2)
            {
                ui->bitrateLabel->setVisible(true);
                ui->sampleRateLabel->setVisible(true);
                ui->bitrateValue->setVisible(true);
                ui->sampleRateValue->setVisible(true);

                ui->resolutionLabel->setVisible(false);
                ui->resolutionValue->setVisible(false);
                ui->fpsLabel->setVisible(false);
                ui->fpsValue->setVisible(false);
            }
            else
            {
                ui->bitrateLabel->setVisible(false);
                ui->sampleRateLabel->setVisible(false);
                ui->bitrateValue->setVisible(false);
                ui->sampleRateValue->setVisible(false);

                ui->resolutionLabel->setVisible(true);
                ui->resolutionValue->setVisible(true);
                ui->fpsLabel->setVisible(true);
                ui->fpsValue->setVisible(true);
            }
       }


        ui->presetsList->setFocus();

    }
}


void EncodeDock::onResetCurrentPreset()
{

    QModelIndex currentIndexModel = ui->presetsList->currentIndex();
    ui->presetsList->clicked(currentIndexModel);
    ui->presetsList->setCurrentIndex(currentIndexModel);
//    ui->presetsList->setFocus();

    resetOptions();
    onProfileChanged();


     bool bVideo = (m_currentSelectedClass!=2)?true:false;
     bool bDisableReset = (m_currentSelectedClass==0)?true:false;
     m_advanceddock->setPreset(m_currentPreset,bVideo,bDisableReset);

}
