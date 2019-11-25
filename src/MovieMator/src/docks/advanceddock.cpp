#include "advanceddock.h"
#include "ui_advanceddock.h"
#include <QComboBox>
#include <QtWidgets>


#define TO_RELATIVE(min, max, abs) qRound(100.0f * float((abs) - (min)) / float((max) - (min) + 1))
AdvancedDock::AdvancedDock(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::AdvancedDock),
   // m_presets(Mlt::Repository::presets()),//预置属性
    m_isDefaultSettings(true)// 是否是默认设置
{

    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());

//    Mlt::Consumer c(MLT.profile(), "avformat");
//    //Q_ASSERT(c.is_valid());
//    if (c.is_valid())
//    {
//    c.set("f", "list");
//    c.set("acodec", "list");
//    c.set("vcodec", "list");
//    c.start();
//    c.stop();

//        Mlt::Properties* p = new Mlt::Properties(c.get_data("f"));

//        Q_ASSERT(p);
//        for (int i = 0; i < p->count(); i++)
//            ui->audioCodecCombo->addItem(p->get(i));
//        delete p;
//        ui->audioCodecCombo->model()->sort(0);
//        ui->audioCodecCombo->insertItem(0, tr("Default for format"));

//        p = new Mlt::Properties(c.get_data("vcodec"));
//        Q_ASSERT(p);
//        for (int i = 0; i < p->count(); i++)
//            ui->videoCodecCombo->addItem(p->get(i));
//        delete p;
//        ui->videoCodecCombo->model()->sort(0);
//        ui->videoCodecCombo->insertItem(0, tr("Default for format"));

//        on_resetButton_clicked();


//  }
}

AdvancedDock::~AdvancedDock()
{
    delete ui;
    //delete m_presets;
}

void AdvancedDock::on_closeButton_clicked()
{
     this->hide();
}
//void EncodeDock::resetOptions()
//{
//    // Reset all controls to default values.
//    //ui->formatCombo->setCurrentIndex(0);
//    //ui->resoulutionvalue->setEnabled(true);
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

//   // on_videoBufferDurationChanged();

//    Mlt::Properties preset;
//    preset.set("f", "mp4");
//    preset.set("movflags", "+faststart");
//    preset.set("vcodec", "libx264");
//    preset.set("crf", "21");
//    preset.set("preset", "fast");
//    preset.set("acodec", "aac");
//    preset.set("meta.preset.extension", "mp4");
//    loadPresetFromProperties(preset);
//}

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
void AdvancedDock::on_resetButton_clicked()
{
    m_isDefaultSettings = true;
    //resetOptions();
//    onProfileChanged();
}

void AdvancedDock::on_scanModeCombo_currentIndexChanged(int index)
{
    //ui->fieldOrderCombo->setDisabled(index);

    ui->deinterlacerCombo->setEnabled(index);
}

void AdvancedDock::on_videoRateControlCombo_activated(int index)
{
    switch (index) {
    case RateControlAverage:
       // ui->videoBitrateCombo->show();
       // ui->videoBufferSizeSpinner->hide();
        ui->videoQualitySpinner->hide();
        //ui->dualPassCheckbox->show();
        //ui->videoBitrateLabel->show();
        //ui->videoBitrateSuffixLabel->show();
        //ui->videoBufferSizeLabel->hide();
        //ui->videoBufferSizeSuffixLabel->hide();
        ui->videoQualityLabel->hide();
        break;
    case RateControlConstant:
       // ui->videoBitrateCombo->show();
       // ui->videoBufferSizeSpinner->show();
        ui->videoQualitySpinner->hide();
        //ui->dualPassCheckbox->show();
        //ui->videoBitrateLabel->show();
       // ui->videoBitrateSuffixLabel->show();
       // ui->videoBufferSizeLabel->show();
       // ui->videoBufferSizeSuffixLabel->show();
        ui->videoQualityLabel->hide();
        break;
    case RateControlQuality:
       // ui->videoBitrateCombo->hide();
        //ui->videoBufferSizeSpinner->hide();
        ui->videoQualitySpinner->show();
        //ui->dualPassCheckbox->hide();
        //ui->videoBitrateLabel->hide();
        //ui->videoBitrateSuffixLabel->hide();
        //ui->videoBufferSizeLabel->hide();
        //ui->videoBufferSizeSuffixLabel->hide();
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
      //  ui->audioQualitySpinner->hide();
        ui->audioBitrateLabel->show();
        ui->audioBitrateSuffixLabel->show();
        //ui->audioQualityLabel->hide();
        break;
    case RateControlConstant:
        ui->audioBitrateCombo->show();
       // ui->audioQualitySpinner->hide();
        ui->audioBitrateLabel->show();
        ui->audioBitrateSuffixLabel->show();
        //ui->audioQualityLabel->hide();
        break;
    case RateControlQuality:
        ui->audioBitrateCombo->hide();
       // ui->audioQualitySpinner->show();
        ui->audioBitrateLabel->hide();
        ui->audioBitrateSuffixLabel->hide();
        //ui->audioQualityLabel->show();
        break;
    }
}

void AdvancedDock::setPreset (Mlt::Properties *preset)
{
       m_currentPreset = preset;
       int audioQuality = -1;
       int videoQuality = -1;
       QStringList other;

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
               ui->sampleRateCombo->lineEdit()->setText(preset->get("ar"));
           else if (name == "ab")
               ui->audioBitrateCombo->lineEdit()->setText(preset->get("ab"));
           else if (name == "vb") {
               ui->videoRateControlCombo->setCurrentIndex(RateControlAverage);
//               ui->videoBitrateCombo->lineEdit()->setText(preset->get("vb"));
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
           else if (name == "aspect") {
               double dar = preset->get_double("aspect");
               switch (int(dar * 100)) {
               case 133:
                   ui->aspectNumSpinner->setValue(4);
                   ui->aspectDenSpinner->setValue(3);
                   break;
               case 177:
                   ui->aspectNumSpinner->setValue(16);
                   ui->aspectDenSpinner->setValue(9);
                   break;
               case 56:
                   ui->aspectNumSpinner->setValue(9);
                   ui->aspectDenSpinner->setValue(16);
                   break;
               default:
                   ui->aspectNumSpinner->setValue(int(dar * 1000));
                   ui->aspectDenSpinner->setValue(1000);
                   break;
               }
   //            ui->aspectNumSpinner->setEnabled(false);
   //            ui->aspectDenSpinner->setEnabled(false);
           }
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

  }

