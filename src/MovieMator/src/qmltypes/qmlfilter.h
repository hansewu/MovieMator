/*
 * Copyright (c) 2013-2015 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: Dragon-S <15919917852@163.com>
 * Author: wyl <wyl@pylwyl.local>
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

#ifndef FILTER_H
#define FILTER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QRectF>
#include <MltFilter.h>
#include "qmlmetadata.h"
#include <QVector>

class AbstractJob;
class AbstractTask;

struct key_frame_item
{
   double keyFrame;
   QMap<QString, QString> paraMap;
};

class QmlFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isNew READ isNew)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(QStringList presets READ presets NOTIFY presetsChanged)
    Q_PROPERTY(int producerIn READ producerIn)
    Q_PROPERTY(int producerOut READ producerOut)
    Q_PROPERTY(double producerAspect READ producerAspect)
    Q_PROPERTY(double mediaWidth READ mediaWidth)
    Q_PROPERTY(double mediaHeight READ mediaHeight)
    Q_PROPERTY(int keyframeNumber READ getKeyFrameNumber NOTIFY keyframeNumberChanged)

public:
    explicit QmlFilter(Mlt::Filter* mltFilter, /*const*/ QmlMetadata* metadata, QObject *parent = 0);
    ~QmlFilter();

    bool isNew() const { return m_isNew; }
    void setIsNew(bool isNew) { m_isNew = isNew; };
    Mlt::Filter *getMltFilter() {return m_filter;};
    void refreshKeyFrame(const QVector<key_frame_item> &listKeyFrame) ;

    Q_INVOKABLE QString get(QString name);
    Q_INVOKABLE double getDouble(QString name);
    Q_INVOKABLE int getInt(QString name);
    Q_INVOKABLE QRectF getRect(QString name, int position = -1);
    Q_INVOKABLE QRectF getRectOfTextFilter(QString name, int position = -1);//FIXME: 暫時之應用與Text Filter，之後其他濾鏡獲取rect方法統一后移除
    Q_INVOKABLE void set(QString name, QString value);
    Q_INVOKABLE void set(QString name, double value);
    Q_INVOKABLE void set(QString name, int value);
    Q_INVOKABLE void set(QString name, double x, double y, double width, double height, double opacity = 1.0,
                             int position = -1, mlt_keyframe_type keyframeType = mlt_keyframe_type(-1));
    Q_INVOKABLE void set(QString name, const QRectF& rect, double opacity = 1.0,
                         int position = -1, mlt_keyframe_type keyframeType = mlt_keyframe_type(-1));
    QString path() const { return m_path; }
    Q_INVOKABLE void loadPresets();
    QStringList presets() const { return m_presets; }
    Q_INVOKABLE int presetCount() { return m_presets.count(); }
    Q_INVOKABLE QString getPresetWithIndex(int index) { return m_presets.at(index); }
    /// returns the index of the new preset
    Q_INVOKABLE int  savePreset(const QStringList& propertyNames, const QString& name = QString());
    Q_INVOKABLE void deletePreset(const QString& name);
    Q_INVOKABLE void analyze(bool isAudio = false);
    Q_INVOKABLE static int framesFromTime(const QString& time);
    Q_INVOKABLE static QString timeFromFrames(int frames);
    Q_INVOKABLE void getHash();
    int producerIn() const;
    int producerOut() const;
    Q_INVOKABLE void setInAndOut(int in, int out);
    double producerAspect() const;
    double mediaWidth() const;
    double mediaHeight() const;
    Q_INVOKABLE void anim_set(QString name, QString value);
    Q_INVOKABLE void resetProperty(const QString& name);

    //function to set or get key frame para value
//#ifdef MOVIEMATOR_PRO

    Q_INVOKABLE void setKeyFrameParaValue(double frame, QString key, QString value);
    Q_INVOKABLE void setKeyFrameParaRectValue(double frame, QString key, const QRectF& rect, double opacity = 1.0);
    Q_INVOKABLE void removeKeyFrameParaValue(double frame);

    Q_INVOKABLE void removeAllKeyFrame();

    Q_INVOKABLE void removeAllKeyFrame(QString name);
    Q_INVOKABLE QString getKeyFrameParaValue(double frame, QString key);
    Q_INVOKABLE double getKeyFrameParaDoubleValue(double frame, QString key);
    Q_INVOKABLE QRectF getKeyFrameParaRectValue(double frame, QString key);
    Q_INVOKABLE double getPreKeyFrameNum(double currentKeyFrame);
    Q_INVOKABLE double getNextKeyFrameNum(double currentKeyFrame);
    Q_INVOKABLE void combineAllKeyFramePara();
    Q_INVOKABLE bool bKeyFrame(double frame);
    Q_INVOKABLE bool bHasPreKeyFrame(double frame);
    Q_INVOKABLE bool bHasNextKeyFrame(double frame);
    Q_INVOKABLE int  getKeyFrameNumber();
    Q_INVOKABLE int  getKeyFrame(int index);
    Mlt::Animation getAnimation(const QString& name);
    Q_INVOKABLE int getKeyFrameCountOnProject(QString name);
    Q_INVOKABLE QString getStringKeyValueOnProjectOnIndex(int index, QString name);
    Q_INVOKABLE int getKeyFrameOnProjectOnIndex(int index, QString name);
    Q_INVOKABLE float getKeyValueOnProjectOnIndex(int index, QString name);

    Q_INVOKABLE QString getAnimStringValue(double frame, QString key);
    Q_INVOKABLE int getAnimIntValue(double frame, QString key);
    Q_INVOKABLE double getAnimDoubleValue(double frame, QString key);
    Q_INVOKABLE QRectF getAnimRectValue(double frame, QString key);
    Q_INVOKABLE void removeAnimationKeyFrame(int nFrame, QString name);

//#endif

    Q_INVOKABLE bool enableAnimation() const { return m_bEnableAnimation; }
    Q_INVOKABLE void setEnableAnimation(bool bEnableAnimation) { m_bEnableAnimation = bEnableAnimation; };

    Q_INVOKABLE bool autoAddKeyFrame() const { return m_bAutoAddKeyFrame; }
    Q_INVOKABLE void setAutoAddKeyFrame(bool bAutoAddKeyFrame) { m_bAutoAddKeyFrame = bAutoAddKeyFrame; };

public slots:
    void preset(const QString& name);

signals:
    void presetsChanged();
    void analyzeFinished(bool isSuccess);
    void changed(); /// Use to let UI and VUI QML signal updates to each other.

//#ifdef MOVIEMATOR_PRO
    void addKeyFrame();
    void removeKeyFrame();
    void keyFrameChanged();
    void keyframeNumberChanged();
//#endif

private:
    QmlMetadata* m_metadata;
    Mlt::Filter* m_filter;
    QString m_path;
    bool m_isNew;
    QStringList m_presets;
    
    QString objectNameOrService();
    double getPreKeyFrameNumInParent(double currentKeyFrame);


    QVector<key_frame_item> m_keyFrameList;
    bool m_bEnableAnimation;
    bool m_bAutoAddKeyFrame;
};

class AnalyzeDelegate : public QObject
{
    Q_OBJECT
public:
    explicit AnalyzeDelegate(Mlt::Filter *filter);

public slots:
    void onAnalyzeFinished(AbstractTask *task, bool isSuccess);

private:
    Mlt::Filter m_filter;
};

#endif // FILTER_H
