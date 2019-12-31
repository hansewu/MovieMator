#include "advanceddock.h"
#include "ui_advanceddock.h"
#include <QComboBox>
#include <QtWidgets>
#include "mltcontroller.h"
#include "dialogs/addencodepresetdialog.h"


#define TO_RELATIVE(min, max, abs) qRound(100.0f * float((abs) - (min)) / float((max) - (min) + 1))
#define TO_ABSOLUTE(min, max, rel) qRound(float(min) + float((max) - (min) + 1) * float(rel) / 100.0f)

AdvancedDock::AdvancedDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::AdvancedDock),
   // m_presets(Mlt::Repository::presets()),//预置属性
    m_isDefaultSettings(true)// 是否是默认设置
{

    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());

    Mlt::Consumer c(MLT.profile(), "avformat");
    //Q_ASSERT(c.is_valid());
    if (c.is_valid())
    {
        c.set("f", "list");
        c.set("acodec", "list");
        c.set("vcodec", "list");
        c.start();
        c.stop();

        Mlt::Properties* p = new Mlt::Properties(c.get_data("acodec"));

        Q_ASSERT(p);
        for (int i = 0; i < p->count(); i++)
            ui->audioCodecCombo->addItem(p->get(i));
        delete p;
        ui->audioCodecCombo->model()->sort(0);
        ui->audioCodecCombo->insertItem(0, tr("Default for format"));

        p = new Mlt::Properties(c.get_data("vcodec"));
        Q_ASSERT(p);
        for (int i = 0; i < p->count(); i++)
            ui->videoCodecCombo->addItem(p->get(i));
        delete p;
        ui->videoCodecCombo->model()->sort(0);
        ui->videoCodecCombo->insertItem(0, tr("Default for format"));
        ui->videoCodecThreadsSpinner->setMaximum(QThread::idealThreadCount());

        //xjp, 给每个控件设置背景色，解决默认字体颜色时控件内容看不清楚的问题
        //video tab  QSpinBox::up-button{background-color: rgb(82, 82, 82)};border:1px solid #aaaaaa;}");
        ui->resolutionBox->setBackgroundRole(QPalette::ColorRole::Dark);
        ui->widthSpinner->setStyleSheet("QSpinBox{background-color: rgb(0, 0, 0); border:1px solid #aaaaaa;}"); \
                                      //  QSpinBox::up-arrow{background-color: rgb(255, 255, 255)};border:1px solid #aaaaaa;}");
        ui->heightSpinner->setStyleSheet("QSpinBox{background-color: rgb(0, 0, 0); border:1px solid #aaaaaa;}");
        ui->fpsSpinner->setStyleSheet("QDoubleSpinBox{background-color: rgb(0, 0, 0); border:1px solid #aaaaaa;}");
        ui->scanModeCombo->setBackgroundRole(QPalette::ColorRole::Dark);
        ui->deinterlacerCombo->setBackgroundRole(QPalette::ColorRole::Dark);
        ui->interpolationCombo->setBackgroundRole(QPalette::ColorRole::Dark);

        //Codec Tab
        ui->videoCodecCombo->setBackgroundRole(QPalette::ColorRole::Dark);
        ui->videoRateControlCombo->setBackgroundRole(QPalette::ColorRole::Dark);
        ui->videoQualitySpinner->setStyleSheet("QSpinBox{background-color: rgb(0, 0, 0); border:1px solid #aaaaaa;}");
        ui->gopSpinner->setStyleSheet("QSpinBox{background-color: rgb(0, 0, 0); border:1px solid #aaaaaa;}");
        ui->bFramesSpinner->setStyleSheet("QSpinBox{background-color: rgb(0, 0, 0); border:1px solid #aaaaaa;}");
        ui->videoCodecThreadsSpinner->setStyleSheet("QSpinBox{background-color: rgb(0, 0, 0); border:1px solid #aaaaaa;}");
        ui->disableVideoCheckbox->setStyleSheet("QCheckBox{background-color: rgb(0, 0, 0);}");

        //Audio Tab
        ui->sampleRateCombo->setBackgroundRole(QPalette::ColorRole::Dark);
        ui->audioCodecCombo->setBackgroundRole(QPalette::ColorRole::Dark);
        ui->audioRateControlCombo->setBackgroundRole(QPalette::ColorRole::Dark);
        ui->audioBitrateCombo->setBackgroundRole(QPalette::ColorRole::Dark);
        ui->disableAudioCheckbox->setStyleSheet("QCheckBox{background-color: rgb(0, 0, 0);}");
        setAttribute(Qt::WA_ShowModal, true);
  }
}

AdvancedDock::~AdvancedDock()
{
    delete ui;
    //delete m_presets;
}



//FIXME: 要支持获取单个clip或者playlist，则需要修改

void AdvancedDock::on_widthSpinner_editingFinished()
{
    //输出分辨率宽强制为4的倍数（解决奇数时导出视频无画面问题）
    int value = (ui->widthSpinner->value() + 3) / 4 * 4;
    ui->widthSpinner->setValue(value);
}

void AdvancedDock::on_heightSpinner_editingFinished()
{
    //输出分辨率高强制为4的倍数（解决奇数时导出视频无画面问题）
    int value = (ui->heightSpinner->value() + 3) / 4 * 4;
    ui->heightSpinner->setValue(value);
}

void AdvancedDock::on_gopSpinner_valueChanged(int value)
{
    Q_UNUSED(value);
    m_isDefaultSettings = false;
}
//void AdvancedDock::on_resetButton_clicked()
//{
//    m_isDefaultSettings = true;
//    //resetOptions();
////    onProfileChanged();
//}

void AdvancedDock::on_scanModeCombo_currentIndexChanged(int index)
{
    //ui->fieldOrderCombo->setDisabled(index);

    ui->deinterlacerCombo->setEnabled(index);
}

void AdvancedDock::on_videoRateControlCombo_activated(int index)
{
    switch (index) {
    case RateControlAverage:
        ui->videoBitrateCombo->show();
        ui->videoBufferSizeSpinner->hide();
        ui->videoQualitySpinner->hide();
     //   ui->dualPassCheckbox->show();
        ui->videoBitrateLabel->show();
        ui->videoBitrateSuffixLabel->show();
        ui->videoBufferSizeLabel->hide();
        ui->videoBufferSizeSuffixLabel->hide();
        ui->videoQualityLabel->hide();
        break;
    case RateControlConstant:
        ui->videoBitrateCombo->show();
        ui->videoBufferSizeSpinner->show();
        ui->videoQualitySpinner->hide();
    //    ui->dualPassCheckbox->show();
        ui->videoBitrateLabel->show();
        ui->videoBitrateSuffixLabel->show();
        ui->videoBufferSizeLabel->show();
        ui->videoBufferSizeSuffixLabel->show();
        ui->videoQualityLabel->hide();
        break;
    case RateControlQuality:
        ui->videoBitrateCombo->hide();
        ui->videoBufferSizeSpinner->hide();
        ui->videoQualitySpinner->show();
   //     ui->dualPassCheckbox->hide();
        ui->videoBitrateLabel->hide();
        ui->videoBitrateSuffixLabel->hide();
        ui->videoBufferSizeLabel->hide();
        ui->videoBufferSizeSuffixLabel->hide();
        ui->videoQualityLabel->show();
        break;
    }
}

void AdvancedDock::on_audioRateControlCombo_activated(int index)
{
    if (ui->audioCodecCombo->currentText() == "libopus")
        // libopus does not use % for quality
        return;
    switch (index) {
    case RateControlAverage:
        ui->audioBitrateCombo->show();
        ui->audioQualitySpinner->hide();
        ui->audioBitrateLabel->show();
        ui->audioBitrateSuffixLabel->show();
        ui->audioQualityLabel->hide();
        break;
    case RateControlConstant:
        ui->audioBitrateCombo->show();
        ui->audioQualitySpinner->hide();
        ui->audioBitrateLabel->show();
        ui->audioBitrateSuffixLabel->show();
        ui->audioQualityLabel->hide();
        break;
    case RateControlQuality:
        ui->audioBitrateCombo->hide();
        ui->audioQualitySpinner->show();
        ui->audioBitrateLabel->hide();
        ui->audioBitrateSuffixLabel->hide();
        ui->audioQualityLabel->show();
        break;
    }
}

void AdvancedDock::setPreset (Mlt::Properties *preset, bool bVideo, bool bDisableReset)
{
       m_currentPreset = preset;
       int audioQuality = -1;
       int videoQuality = -1;
       QStringList other;

       ui->deinterlacerCombo->setEnabled(true);
       ui->deinterlacerCombo->setCurrentIndex(3);

       ui->interpolationCombo->setEnabled(true);
       ui->interpolationCombo->setCurrentIndex(1);
       ui->videoBitrateCombo->lineEdit()->setText("2M");
       ui->videoBufferSizeSpinner->setValue(224);
       ui->gopSpinner->blockSignals(true);
       ui->gopSpinner->setValue(13);
       ui->gopSpinner->blockSignals(false);
       ui->bFramesSpinner->setValue(2);
       ui->videoCodecThreadsSpinner->setValue(0);
   //    ui->dualPassCheckbox->setChecked(false);
       ui->disableVideoCheckbox->setChecked(false);

       ui->sampleRateCombo->lineEdit()->setText("44100");
       ui->audioBitrateCombo->lineEdit()->setText("384k");
       ui->audioQualitySpinner->setValue(50);

       ui->scanModeCombo->setEnabled(true);

       //xjp 2019.12.6 用profile的宽，高来设置widthSpinner， heightSpinner的初始值
       int width = MLT.profile().width();
       int height = MLT.profile().height();

       ui->widthSpinner->setValue(width);
       ui->heightSpinner->setValue(height);

//       ui->resetPresetButton->setEnabled(!bDisableReset);

   //    ui->videoRateControlCombo->setCurrentIndex(RateControlQuality);

       //****** end ******

      // ui->presetLabel->setText(preset.get("meta.preset.name"));


       ui->disableAudioCheckbox->setChecked(preset->get_int("an"));
       ui->disableVideoCheckbox->setChecked(preset->get_int("vn"));
       for (int i = 0; i < preset->count(); i++) {
           QString name(preset->get_name(i));

            if (name == "acodec") {
               for (int i = 0; i < ui->audioCodecCombo->count(); i++)
                   if (ui->audioCodecCombo->itemText(i) == preset->get("acodec"))
                       ui->audioCodecCombo->setCurrentIndex(i);
               if (ui->audioCodecCombo->currentText() == "libopus")
                   // reset libopus to VBR (its default)
                   ui->audioRateControlCombo->setCurrentIndex(RateControlQuality);
           }
           else if (name == "vcodec") {
               for (int i = 0; i < ui->videoCodecCombo->count(); i++)
                   if (ui->videoCodecCombo->itemText(i) == preset->get("vcodec"))
                       ui->videoCodecCombo->setCurrentIndex(i);
           }
           else if (name == "ar")
            {

               ui->sampleRateCombo->lineEdit()->setText(preset->get("ar"));
            }
           else if (name == "ab")
               ui->audioBitrateCombo->lineEdit()->setText(preset->get("ab"));
           else if (name == "vb") {
               ui->videoRateControlCombo->setCurrentIndex(RateControlAverage);
               ui->videoBitrateCombo->lineEdit()->setText(preset->get("vb"));
           }
           else if (name == "g")
               ui->gopSpinner->setValue(preset->get_int("g"));
           else if (name == "bf")
               ui->bFramesSpinner->setValue(preset->get_int("bf"));
           else if (name == "deinterlace") {
               ui->scanModeCombo->setCurrentIndex(preset->get_int("deinterlace"));
               ui->scanModeCombo->setEnabled(false);
           }
           else if (name == "progressive") {
               ui->scanModeCombo->setCurrentIndex(preset->get_int("progressive"));
               ui->scanModeCombo->setEnabled(false);
           }
//           else if (name == "top_field_first") {
//               ui->fieldOrderCombo->setCurrentIndex(preset->get_int("top_field_first"));
//           }
           else if (name == "width") {
               ui->widthSpinner->setValue(preset->get_int("width"));
   //            ui->widthSpinner->setEnabled(false);
           }
           else if (name == "height") {
               ui->heightSpinner->setValue(preset->get_int("height"));
   //            ui->heightSpinner->setEnabled(false);
           }

//           }+
           else if (name == "r") {
               ui->fpsSpinner->setValue(preset->get_double("r"));
   //            ui->fpsSpinner->setEnabled(false);
           }
           else if (name == "pix_fmt") {
               QString pix_fmt(preset->get("pix_fmt"));
   //            if (pix_fmt != "yuv420p")
                   other.append(QString("%1=%2").arg(name).arg(pix_fmt));
           }
           else if (name == "aq") {
               ui->audioRateControlCombo->setCurrentIndex(RateControlQuality);
               audioQuality = preset->get_int("aq");
           }
           else if (name == "vbr") {
               // libopus rate mode
               QString value(preset->get("vbr"));
               if (value == "off")
                   ui->audioRateControlCombo->setCurrentIndex(RateControlConstant);
               else if (value == "constrained")
                   ui->audioRateControlCombo->setCurrentIndex(RateControlAverage);
               else
                   ui->audioRateControlCombo->setCurrentIndex(RateControlQuality);
           }
           else if (name == "vq") {
               ui->videoRateControlCombo->setCurrentIndex(RateControlQuality);
               videoQuality = preset->get_int("vq");
           }
           else if (name == "qscale") {
               ui->videoRateControlCombo->setCurrentIndex(RateControlQuality);
               videoQuality = preset->get_int("qscale");
           }
           else if (name == "crf") {
               ui->videoRateControlCombo->setCurrentIndex(RateControlQuality);
               videoQuality = preset->get_int("crf");
           }
           else if (name == "bufsize") {
               // traditionally this means video only
               ui->videoRateControlCombo->setCurrentIndex(RateControlConstant);
               //ui->videoBufferSizeSpinner->setValue(getBufferSize(preset, "bufsize"));
           }
           else if (name == "vbufsize") {
               ui->videoRateControlCombo->setCurrentIndex(RateControlConstant);
              // ui->videoBufferSizeSpinner->setValue(getBufferSize(preset, "vbufsize"));
           }
           else if (name == "x265-params") {
               QStringList x265params = QString::fromUtf8(preset->get("x265-params")).split(':');
               foreach (QString param, x265params) {
                   QStringList nameValue = param.split('=');
                   if ("vbv-bufsize" == nameValue[0] && nameValue.size() > 1) {
                       ui->videoRateControlCombo->setCurrentIndex(RateControlConstant);
                       // convert from Kb to KB
                       //ui->videoBufferSizeSpinner->setValue(nameValue[1].toDouble() / 8);
                       break;
                   }
               }
           }
           else if (name == "threads") {
               // TODO: should we save the thread count and restore it if preset is not 1?
               if (preset->get_int("threads") == 1)
                   ui->videoCodecThreadsSpinner->setValue(1);
           }

           else if (name == "deinterlace_method") {
               name = preset->get("deinterlace_method");
               if (name == "onefield")
                   ui->deinterlacerCombo->setCurrentIndex(0);
               else if (name == "linearblend")
                   ui->deinterlacerCombo->setCurrentIndex(1);
               else if (name == "yadif-nospatial")
                   ui->deinterlacerCombo->setCurrentIndex(2);
               else if (name == "yadif")
                   ui->deinterlacerCombo->setCurrentIndex(3);
               ui->deinterlacerCombo->setDisabled(true);
           }
           else if (name == "rescale") {
               name = preset->get("rescale");
               if (name == "nearest" || name == "neighbor")
                   ui->interpolationCombo->setCurrentIndex(0);
               else if (name == "bilinear")
                   ui->interpolationCombo->setCurrentIndex(1);
               else if (name == "bicubic")
                   ui->interpolationCombo->setCurrentIndex(2);
               else if (name == "hyper" || name == "lanczos")
                   ui->interpolationCombo->setCurrentIndex(3);
               ui->interpolationCombo->setDisabled(true);
           }
           else if (name != "an" && name != "vn" && name != "threads"
                    && !name.startsWith('_') && !name.startsWith("meta.preset.")) {
               other.append(QString("%1=%2").arg(name).arg(preset->get(i)));
           }
       }
      // ui->advancedTextEdit->setPlainText(other.join("\n"));

//        normalize the quality settings
//        quality depends on codec
//       if (ui->audioRateControlCombo->currentIndex() == RateControlQuality && audioQuality > -1) {
//           const QString& acodec = ui->audioCodecCombo->currentText();
//           if (acodec == "libmp3lame") // 0 (best) - 9 (worst)
//               ui->audioQualitySpinner->setValue(TO_RELATIVE(9, 0, audioQuality));
//           if (acodec == "libvorbis" || acodec == "vorbis") // 0 (worst) - 10 (best)
//               ui->audioQualitySpinner->setValue(TO_RELATIVE(0, 10, audioQuality));
//           //else
//                aac: 0 (worst) - 500 (best)
//               ui->audioQualitySpinner->setValue(TO_RELATIVE(0, 500, audioQuality));
//       }


       //xjp 2019.12.9判断目标分辨率是否在分辨率列表中，
       QString resolutionStr = QString("%1x%2").arg(ui->widthSpinner->value()).arg(ui->heightSpinner->value());
       int resolutionCount = ui->resolutionBox->count();
       int index=0;

       for(index = 0; index < resolutionCount; index++)
       {
           if(ui->resolutionBox->itemText(index).contains(resolutionStr))
           {
               ui->resolutionBox->setCurrentIndex(index);
               break;
           }

       }

       if(index == resolutionCount)
           ui->resolutionBox->setCurrentIndex(resolutionCount-1);

       //********* end **********


       int frame_rate_num = m_currentPreset->get_int("frame_rate_num");
       int frame_rate_den = m_currentPreset->get_int("frame_rate_den");


       if (frame_rate_num == 24000 && frame_rate_den == 1001)
       {
            ui->fpsSpinner->setValue(23.98);

       }
       else if (frame_rate_num == 30000 && frame_rate_den == 1001)
       {
           ui->fpsSpinner->setValue(29.97);
       }
       else if (frame_rate_num == 48000 && frame_rate_den == 1001)
       {
           ui->fpsSpinner->setValue(47.97);
       }
       else if (frame_rate_num == 60000 && frame_rate_den == 1001)
       {
           ui->fpsSpinner->setValue(59.94);

       }


       if (ui->videoRateControlCombo->currentIndex() == RateControlQuality && videoQuality > -1) {
           const QString& vcodec = ui->videoCodecCombo->currentText();
           //val = min + (max - min) * paramval;
           if (vcodec == "libx264" || vcodec == "libx265") // 0 (best, 100%) - 51 (worst)
               ui->videoQualitySpinner->setValue(TO_RELATIVE(51, 0, videoQuality));
           else if (vcodec.startsWith("libvpx")) // 0 (best, 100%) - 63 (worst)
               ui->videoQualitySpinner->setValue(TO_RELATIVE(63, 0, videoQuality));
           else  //1 (best, NOT 100%) - 31 (worst)
               ui->videoQualitySpinner->setValue(TO_RELATIVE(31, 1, videoQuality));
       }

           on_audioRateControlCombo_activated(ui->audioRateControlCombo->currentIndex());
           on_videoRateControlCombo_activated(ui->videoRateControlCombo->currentIndex());

           if(bVideo)
           {
               if(ui->tabWidget->count() == 1)
               {
                   ui->tabWidget->insertTab(0, ui->videoTab,"Video");
                   ui->tabWidget->insertTab(1, ui->codecTab,"Codec");
               }
               ui->tabWidget->setCurrentIndex(0);

           }
           else
           {
             //  ui->tabWidget->setCurrentIndex(2);
               if(ui->tabWidget->count() == 3)
               {
                   ui->tabWidget->removeTab(0);
                   ui->tabWidget->removeTab(0);//xjp, 2019.12.19当删除第一个tab后，第二个tab的index自动变为0了，所以序号还是0

               }
           }

  }


void AdvancedDock::on_resolutionBox_currentIndexChanged(const QString &arg1)
{

    if(arg1 == "Custom")
        return;

    //xjp 2019.12.6 将选中的分辨率中的宽，高分离出来，写入下面的宽，高spinner中
    QStringList textParts = arg1.split('x');
    QStringList textParts2 = textParts[1].split('(');

    ui->widthSpinner->setValue(textParts.at(0).toInt())  ;
    ui->heightSpinner->setValue(textParts2.at(0).toInt());

}




void AdvancedDock::on_cancelButton_clicked()
{
    this->hide();
}

void AdvancedDock::on_okButton_clicked()
{
    int threadCount = QThread::idealThreadCount();
    if (threadCount > 2 )//&& ui->parallelCheckbox->isChecked()
        threadCount = qMin(threadCount - 1, 4);
    else
        threadCount = 1;
    updateCurrentPreset(-threadCount);

    QString strFps = QString::number(ui->fpsSpinner->value(),10,3);
    emit updateAdvancedSetting(strFps);
    this->hide();
}


void AdvancedDock::updateCurrentPreset(int realtime)
{

    if (m_currentPreset && m_currentPreset->is_valid())
    {
        // foreach (QString line, ui->advancedTextEdit->toPlainText().split("\n"))
        //     p->parse(line.toUtf8().constData());
        if (realtime)
            m_currentPreset->set("real_time", realtime);
        // p->set("meta.preset.name", ui->presetLabel->text().toLatin1().constData());
//        if (ui->formatCombo->currentIndex() != 0)
//            m_currentPreset->set("f", ui->formatCombo->currentText().toLatin1().constData());
        if (ui->disableAudioCheckbox->isChecked())
        {
            m_currentPreset->set("an", 1);
            m_currentPreset->set("audio_off", 1);
        }
        else
        {
            const QString& acodec = ui->audioCodecCombo->currentText();
            if (ui->audioCodecCombo->currentIndex() > 0)
                m_currentPreset->set("acodec", ui->audioCodecCombo->currentText().toLatin1().constData());
            m_currentPreset->set("ar", ui->sampleRateCombo->currentText().toLatin1().constData());


            if (ui->audioRateControlCombo->currentIndex() == RateControlAverage
                    || ui->audioRateControlCombo->currentIndex() == RateControlConstant)
            {
                m_currentPreset->set("ab", ui->audioBitrateCombo->currentText().toLatin1().constData());
                if (acodec == "libopus") {
                    if (RateControlConstant == ui->audioRateControlCombo->currentIndex())
                        m_currentPreset->set("vbr", "off");
                    else
                        m_currentPreset->set("vbr", "constrained");
                }
            } else if (acodec == "libopus") {
                m_currentPreset->set("vbr", "on");
                m_currentPreset->set("ab", ui->audioBitrateCombo->currentText().toLatin1().constData());
            } else {
                int aq = ui->videoQualitySpinner->value();
                if (acodec == "libmp3lame")
                    aq = TO_ABSOLUTE(9, 0, aq);
                else if (acodec == "libvorbis" || acodec == "vorbis")
                    aq = TO_ABSOLUTE(0, 10, aq);
                else
                    aq = TO_ABSOLUTE(0, 500, aq);
                m_currentPreset->set("aq", aq);
            }
        }
        if (ui->disableVideoCheckbox->isChecked())
        {
            m_currentPreset->set("vn", 1);
            m_currentPreset->set("video_off", 1);
        }
        else
        {
            const QString& vcodec = ui->videoCodecCombo->currentText();
            if (ui->videoCodecCombo->currentIndex() > 0)
                m_currentPreset->set("vcodec", vcodec.toLatin1().constData());
            if (vcodec == "libx265")
            {
                // Most x265 parameters must be supplied through x265-params.
                QString x265params = QString::fromUtf8(m_currentPreset->get("x265-params"));
                switch (ui->videoRateControlCombo->currentIndex())
                {
                case RateControlAverage:
                    m_currentPreset->set("vb", ui->audioBitrateCombo->currentText().toLatin1().constData());
                    break;
                case RateControlConstant:
                {
                    QString b = ui->audioBitrateCombo->currentText();
                    // x265 does not expect bitrate suffixes and requires Kb/s
                    b.replace('k', "").replace('M', "000");
                    x265params = QString("bitrate=%1:vbv-bufsize=%2:vbv-maxrate=%3:%4")
                        .arg(b).arg(int(ui->videoBufferSizeSpinner->value() * 8)).arg(b).arg(x265params);
                    break;
                }
                case RateControlQuality:
                {
                    int vq = ui->videoQualitySpinner->value();
                    x265params = QString("crf=%1:%2").arg(TO_ABSOLUTE(51, 0, vq)).arg(x265params);
                    // Also set crf property so that custom presets can be interpreted properly.
                    m_currentPreset->set("crf", TO_ABSOLUTE(51, 0, vq));
                    break;
                }
                }
                x265params = QString("keyint=%1:bframes=%2:%3").arg(ui->gopSpinner->value())
                            .arg(ui->bFramesSpinner->value()).arg(x265params);
                m_currentPreset->set("x265-params", x265params.toUtf8().constData());
            }
            else
            {
                switch (ui->videoRateControlCombo->currentIndex())
                {
                    case RateControlAverage:
                        m_currentPreset->set("vb", ui->videoBitrateCombo->currentText().toLatin1().constData());
                        break;
                    case RateControlConstant: {
                        const QString& b = ui->videoBitrateCombo->currentText();
                        m_currentPreset->set("vb", b.toLatin1().constData());
                        m_currentPreset->set("vminrate", b.toLatin1().constData());
                        m_currentPreset->set("vmaxrate", b.toLatin1().constData());
                        m_currentPreset->set("vbufsize", int(ui->videoBufferSizeSpinner->value() * 8 * 1024));
                        break;
                        }
                    case RateControlQuality: {
                        int vq = ui->videoQualitySpinner->value();
                        if (vcodec == "libx264") {
                            m_currentPreset->set("crf", TO_ABSOLUTE(51, 0, vq));
                        } else if (vcodec.startsWith("libvpx")) {
                            m_currentPreset->set("crf", TO_ABSOLUTE(63, 0, vq));
                            m_currentPreset->set("vb", 0); // VP9 needs this to prevent constrained quality mode.
                        } else {
                            m_currentPreset->set("qscale", TO_ABSOLUTE(31, 1, vq));
                        }
                        break;
                        }
                  }
            }



            m_currentPreset->set("g", ui->gopSpinner->value());
            m_currentPreset->set("bf", ui->bFramesSpinner->value());
        }
        m_currentPreset->set("width", ui->widthSpinner->value());
        m_currentPreset->set("height", ui->heightSpinner->value());
        m_currentPreset->set("aspect", double(ui->widthSpinner->value()) / double(ui->heightSpinner->value()));
        m_currentPreset->set("progressive", ui->scanModeCombo->currentIndex());
   //         m_currentPreset->set("top_field_first", ui->fieldOrderCombo->currentIndex());


        switch (ui->deinterlacerCombo->currentIndex())
        {
            case 0:
                m_currentPreset->set("deinterlace_method", "onefield");
                break;
            case 1:
                m_currentPreset->set("deinterlace_method", "linearblend");
                break;
            case 2:
                m_currentPreset->set("deinterlace_method", "yadif-nospatial");
                break;
            default:
                m_currentPreset->set("deinterlace_method", "yadif");
                break;
        }
        switch (ui->interpolationCombo->currentIndex())
        {
            case 0:
                m_currentPreset->set("rescale", "nearest");
                break;
            case 1:
                m_currentPreset->set("rescale", "bilinear");
                break;
            case 2:
                m_currentPreset->set("rescale", "bicubic");
                break;
            default:
                m_currentPreset->set("rescale", "hyper");
                break;
        }
        if (qFloor(ui->fpsSpinner->value() * 10.0) == 239)
        {
             m_currentPreset->set("frame_rate_num", 24000);
             m_currentPreset->set("frame_rate_den", 1001);


        }
        else if (qFloor(ui->fpsSpinner->value() * 10.0) == 299)
        {
             m_currentPreset->set("frame_rate_num", 30000);
             m_currentPreset->set("frame_rate_den", 1001);

        }
        else if (qFloor(ui->fpsSpinner->value() * 10.0) == 479)
        {
             m_currentPreset->set("frame_rate_num", 48000);
             m_currentPreset->set("frame_rate_den", 1001);

        }
        else if (qFloor(ui->fpsSpinner->value() * 10.0) == 599)
        {
             m_currentPreset->set("frame_rate_num", 60000);
             m_currentPreset->set("frame_rate_den", 1001);

        }
        else
        {
             m_currentPreset->set("r", qFloor(ui->fpsSpinner->value()));
             m_currentPreset->set("frame_rate_num", qFloor(ui->fpsSpinner->value()));
             m_currentPreset->set("frame_rate_den", 1);
        }


        if (ui->videoCodecCombo->currentText() == "prores" || m_currentPreset->get("meta.preset.name") == "image2")
             m_currentPreset->set("threads", 1);
        else if (ui->videoCodecThreadsSpinner->value() == 0
                     && ui->videoCodecCombo->currentText() != "libx264"
                     && ui->videoCodecCombo->currentText() != "libx265")
             m_currentPreset->set("threads", QThread::idealThreadCount() - 1);
        else
             m_currentPreset->set("threads", ui->videoCodecThreadsSpinner->value());
//        if (ui->dualPassCheckbox->isEnabled() && ui->dualPassCheckbox->isChecked())
//            p->set("pass", 1);
     }
     

}



void AdvancedDock::on_addPresetButton_clicked()
{
    int threadCount = QThread::idealThreadCount();
    if (threadCount > 2 )//&& ui->parallelCheckbox->isChecked()
        threadCount = qMin(threadCount - 1, 4);
    else
        threadCount = 1;

    updateCurrentPreset(-threadCount);

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
            emit addCustomPreset(preset);   //   loadPresets();
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


Mlt::Properties* AdvancedDock::collectProperties(int realtime)
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
}

void AdvancedDock::on_resetPresetButton_clicked()
{
    emit resetCurrentPreset();
}
