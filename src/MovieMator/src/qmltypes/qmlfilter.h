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
   int keyFrame;
   QMap<QString, QString> paraMap;
};

/*!
  \class QmlFilter

  QmlFilter is a wrapper around a part of the MLT library that provides access to
  the project data. QmlFilter is mainly used in qml files.
  QmlFilter can access Mlt::Filer to get data for qml interface display. When the qml interface modifies some parameters, you can modify the mlt library project data through QmlFilter.

*/
class QmlFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isNew READ isNew)
    Q_PROPERTY(QString resourcePath READ resourcePath)
    Q_PROPERTY(QStringList presets READ presets NOTIFY presetsChanged)
    Q_PROPERTY(int producerIn READ producerIn)
    Q_PROPERTY(int producerOut READ producerOut)
//    Q_PROPERTY(double producerAspect READ producerAspect)
    Q_PROPERTY(double mediaWidth READ mediaWidth)
    Q_PROPERTY(double mediaHeight READ mediaHeight)
    Q_PROPERTY(int keyframeNumber READ cache_getKeyFrameNumber NOTIFY keyframeNumberChanged)

    //Q_PROPERTY(int isKeyframeActivate READ isKeyframeActivate NOTIFY isKeyframeActivateChanged)

public:
    explicit QmlFilter(Mlt::Filter* mltFilter, /*const*/ QmlMetadata* metadata, QObject *parent = nullptr);
    ~QmlFilter();

    /** A Boolean value that indicates whether the filter is new.
     *
     * \return true when the filter is new; otherwise, false
     */
    bool isNew() const { return m_isNew; }

    /** Sets the filter’s new property to the value specified by isNew.
     *
     * \param isNew the filter is new or not
     */
    void setIsNew(bool isNew) { m_isNew = isNew; }

    /** Get the filter.
     *
     * \return a Mlt::Filter value
     */
    Mlt::Filter *getMltFilter() {return m_filter;}

    /** Update the filter’s all key frames to the value specified by listKeyFrame.
     * Sync cache data to project
     * \param listKeyFrame the filter's new key frames
     */
    void refreshKeyFrame(const QVector<key_frame_item> &listKeyFrame) ;

    /** Update the filter’s all parameters to the value specified by listParameter.
     * Sync cache data to project
     * \param listParameter the filter's new data
     */
    void refreshNoAnimation(const QVector<key_frame_item> &listParameter, bool bFromUndo = false) ;

    /** Get a string value by name of Mlt::Filter.
     *
     * \param name the property to get
     * \return the property's string value or NULL if it does not exist
     */
    Q_INVOKABLE QString get(QString name);

    /** Get a floating point value associated to the name of Mlt::Filter.
     *
     * \param name the property to get
     * \return the floating point, 0 if not found (which may also be a legitimate value)
     */
    Q_INVOKABLE double getDouble(QString name);

    /** Get an integer associated to the name of Mlt::Filter.
     *
     * \param name the property to get
     * \return The integer value, 0 if not found (which may also be a legitimate value)
     */
    Q_INVOKABLE int getInt(QString name);

    /** Get a QRectF value associated to the name of Mlt::Filter or get a QRectF value associated to the name at a frame position.
     *
     * \param name the property to get
     * \param position the frame number, when position = -1 get a QRectF value associated to the name
     * \return the QRectF value, QRectF(0.0, 0.0, 0.0, 0.0) if not found
     */
    Q_INVOKABLE QRectF getRect(QString name, int position = -1);

    /** Get a QRectF value associated to the name of text filter or get a QRectF value associated to the name at a frame position.
     *
     * \param name the property to get
     * \param position the frame number, when position = -1 get a QRectF value associated to the name
     * \return the QRectF value, QRectF() if not found
     */
    Q_INVOKABLE QRectF getRectOfTextFilter(QString name, int position = -1);//FIXME: 暫時之應用與Text Filter，之後其他濾鏡獲取rect方法統一后移除

    /** Set a property to a string value of Mlt::Filter.
     *
     * \param name the property to set
     * \param value the string
     */
    Q_INVOKABLE void set(QString name, QString value);

    /** Set a property to a floating point value of Mlt::Filter.
     *
     * \param name the property to set
     * \param value the floating point
     */
    Q_INVOKABLE void set(QString name, double value);

    /** Set a property to an integer value.
     *
     * \param name the property to set
     * \param value the integer
     */
    Q_INVOKABLE void set(QString name, int value);

    /** Set a property to values with coordinates, size, and opacity of Mlt::Filter.
     *
     * \param name the property to set
     * \param x X coordinate
     * \param y Y coordinate
     * \param width width
     * \param height height
     * \param opacity the opacity value
     * \param position the frame number, when position = -1 set a QRectF value associated to the name，but it did not implement this function
     * \param keyframeType the method of interpolation for this key frame
     */
    Q_INVOKABLE void set(QString name, double x, double y, double width, double height, double opacity = 1.0,
                             int position = -1, mlt_keyframe_type keyframeType = mlt_keyframe_type(-1));

    /** Set a QRectF value associated to the name of Mlt::Filter.
     *
     * \param name the property to set
     * \param rect the QRectF value
     * \param opacity the opacity value
     * \param position the frame number, when position = -1 set a QRectF value associated to the name，but it did not implement this function
     * \param keyframeType the method of interpolation for this key frame
     */
    Q_INVOKABLE void set(QString name, const QRectF& rect, double opacity = 1.0,
                         int position = -1, mlt_keyframe_type keyframeType = mlt_keyframe_type(-1));

    /** Fetch the metadata directory of the Mlt::Filter.
     *
     * \return the absolute path to the metadata directory that this Mlt::Filter is using
     */
    QString resourcePath() const { return m_sResourcePath; }

    /** Load Mlt::Filter's presets.
     *  set Mlt::Filter's preset files
     */
    Q_INVOKABLE void loadPresets();

    /** Fetch the names of Mlt::Filter'presets files.
     *
     * \return the names of preset files
     */
    QStringList presets() const { return m_presets; }

    /** Fetch the number of Mlt::Filter's preset file.
     *
     * \return the number of preset file
     */
    Q_INVOKABLE int presetCount() { return m_presets.count(); }

    /** Get the name for the N-th preset.
     *
     * \param index the N-th preset (0 based)
     * \return The QString value
     */
    Q_INVOKABLE QString getPresetWithIndex(int index) { return m_presets.at(index); }

    /** Save properties as a new preset.
     *
     * \param propertyNames the properties's name and value
     * \param name the preset name
     * \return the index of the new preset
     */
    Q_INVOKABLE int  savePreset(const QStringList& propertyNames, const QString& name = QString());

    /** remove a preset by name.
     *
     * \param name the preset name
     */
    Q_INVOKABLE void deletePreset(const QString& name);

    /** Analyze the application of consumer and filter on the clip.
     *
     * \param isAudio A boolean value that indicates whether to analyze the video or the audio
     */
    Q_INVOKABLE void analyze(bool isAudio = false);

    /** Convert a time string to a frame count.
     *
     * \param time the time string to convert
     * \return a frame count or n <= 0 if error, e.g. there is no profile
     */
    Q_INVOKABLE static int framesFromTime(const QString& time);

    /** Convert a frame count to a time string.
     *
     * \param frames the frame count to convert
     * \return the time string or NULL if error, e.g. there is no profile
     */
    Q_INVOKABLE static QString timeFromFrames(int frames);

    /** Get Hash of Mlt::Filter.
     *
     */
    Q_INVOKABLE void getHash();

    /** Get the in point of the filter's producer.
     *
     * \return the in point
     */
    int producerIn() const;

    /** Get the out point of the filter's producer.
     *
     * \return the out point
     */
    int producerOut() const;

    /** Set the starting and ending time.
     *
     * \param in the time relative to the producer at which start applying the filter
     * \param out the time relative to the producer at which to stop applying the filter
     */
    Q_INVOKABLE void setInAndOut(int in, int out);

    /** Get the display aspect ratio as floating point value.
     *
     * \return the image aspect ratio
     */
    Q_INVOKABLE double producerAspect() const;

    /** Get the horizontal resolution of the video.
    *
    * \return The double value
    */
    double mediaWidth() const;

    /** Get the vertical resolution of the video.
    *
    * \return The double value
    */
    double mediaHeight() const;

    /** Set a property to a string value of Mlt::Filter.
     * Expiration method
     * \param name the property to set
     * \param value the string
     */
    Q_INVOKABLE void anim_set(QString name, QString value);

    /** Remove the value for a property.
     *
     * This initializes the value to zero and removes any string, data, or animation.
     * This is especially useful when you want to reset an animation.
     * \param name the name of the property to clear
     */
    Q_INVOKABLE void resetProperty(const QString& name);

    //function to set or get key frame para value
//#ifdef MOVIEMATOR_PRO
    /** Set a QString value associated to the name at a frame position to cached data.
     *
     * \param key the property to set
     * \param frame the frame number
     * \param value QString value
     */
    Q_INVOKABLE void cache_setKeyFrameParaValue(int frame, QString key, QString value, bool bFromUndo = false);

    /** Set a QRectF value associated to the name at a frame position to cached data.
     *
     * \param key the property to set
     * \param frame the frame number
     * \param rect QRectF value
     * \param opacity the opacity value
     */
    Q_INVOKABLE void cache_setKeyFrameParaRectValue(int frame, QString key, const QRectF& rect, double opacity = 1.0);

    /** Remove the keyframe at the specified position.
     *
     * \param frame the frame number in the clip of the animation node to remove
     */
    Q_INVOKABLE void removeKeyFrameParaValue(int frame, bool bFromUndo = false);

    /** Remove all the key frames of filter.
     *
     */
    Q_INVOKABLE void removeAllKeyFrame(bool bFromUndo = false);

    /** Remove all the key frames of filter.
     *
     * \param name the property to remove
     */
    Q_INVOKABLE void removeAllKeyFrame(QString name, bool bFromUndo = false);

    /** Get a string value by name at a frame position from cached data.
     *
     * \param key the property to get
     * \param frame the frame number
     * \return the property's string value or "" if it does not exist
     */
    Q_INVOKABLE QString cache_getKeyFrameParaValue(int frame, QString key);

    /** Get a double value by name at a frame position from cached data.
     *
     * \param key the property to get
     * \param frame the frame number
     * \return the property's double value or -1 if it does not exist
     */
    Q_INVOKABLE double cache_getKeyFrameParaDoubleValue(int frame, QString key);

    /** Get a QRectF value by name at a frame position from cached data.
     *
     * \param key the property to get
     * \param frame the frame number
     * \return the property's QRectF value or QRectF() if it does not exist
     */
    Q_INVOKABLE QRectF cache_getKeyFrameParaRectValue(int frame, QString key);

    /** Get the keyfame at the previous following of the point from cached data.
     *
     * \param currentKeyFrame the frame number at which to start looking for the previous animation node
     * \return the integer keyframe, -1 if not found or not in the clip
     */
    Q_INVOKABLE int cache_getPreKeyFrameNum(int currentKeyFrame, QString propertyName = nullptr);

    /** Get the keyfame at the next following of the point from cached data.
     *
     * \param currentKeyFrame the frame number at which to start looking for the next animation node
     * \return the integer keyframe, -1 if not found or not in the clip
     */
    Q_INVOKABLE int cache_getNextKeyFrameNum(int currentKeyFrame, QString propertyName = nullptr);

    /** Sync all keyfame cache to project.
     *
     */
    Q_INVOKABLE void syncCacheToProject();

    /** Get a boolean of whether this is a key frame or not from cached data.
     *
     * \param frame the frame number in the clip
     * \return true if this is a key frame, false if not a key frame
     */
    Q_INVOKABLE bool cache_bKeyFrame(int frame, QString propertyName = nullptr);

    /** Get a boolean of whether there is a previous frame at the position or not.
     *
     * \param frame the frame number at which to start looking for the previous animation node
     * \return true if there is a previous frame, false if not a previous frame at the position
     */
    Q_INVOKABLE bool bHasPreKeyFrame(int frame);

    /** Get a boolean of whether there is a next frame at the position or not.
     *
     * \param frame the frame number at which to start looking for the next animation node
     * \return true if there is a next frame, false if not a previous frame at the position
     */
    Q_INVOKABLE bool bHasNextKeyFrame(int frame);

    /** Get the number of keyframes from cache data.
     *
     * \return the number of keyframes
     */
    Q_INVOKABLE int  cache_getKeyFrameNumber(QString propertyName = nullptr);

    /** Get the keyfame number for the N-th keyframe.
     *
     * \param index the N-th keyframe (0 based) in this animation
     * \return The integer value, -1 if not found (which may also be a legitimate value)
     */
    Q_INVOKABLE int  getKeyFrame(int index, QString propertyName = nullptr);

    /** Get the animation associated to the name.
     *
     * \param name the property to get
     * \return The animation object or Mlt::Animation() if the property has no animation
     */
    Mlt::Animation getAnimation(const QString& name);

    /** Get the number of keyframes.
     *
     * \param name the property to get
     * \return the number of keyframes
     */
    Q_INVOKABLE int getKeyFrameCountOnProject(QString name);

    /** Get a QString value by name for the N-th keyframe
     *
     * \param name the property to get
     * \param index the N-th keyframe (0 based) in this animation
     * \return the property's QString value or "" if it does not exist
     */
    Q_INVOKABLE QString getStringKeyValueOnProjectOnIndex(int index, QString name);

    /** Get the frame number by name for the N-th keyframe
     *
     * \param name the property to get
     * \param index the N-th keyframe (0 based) in this animation
     * \return the frame number or -1 if it does not exist or not in the clip range
     */
    Q_INVOKABLE int getKeyFrameOnProjectOnIndex(int index, QString name);

    /** Get a doulbe value by name for the N-th keyframe.
     *
     * \param name the property to get
     * \param index the N-th keyframe (0 based) in this animation
     * \return the property'double value or 0 if it does not exist
     */
    Q_INVOKABLE double getKeyValueOnProjectOnIndex(int index, QString name);

    /** Get a QString value by name at a frame position.
     *
     * \param frame the frame number
     * \param key the property to get
     * \return the property'QString value or QString() if it does not exist
     */
    Q_INVOKABLE QString getAnimStringValue(int frame, QString key);

    /** Get an integer associated to the name at a frame position.
     *
     * \param frame the frame number
     * \param key the property to get
     * \return the property'integer value or -1 if not found
     */
    Q_INVOKABLE int getAnimIntValue(int frame, QString key);

    /** Get a double associated to the name at a frame position.
     *
     * \param frame the frame number
     * \param key the property to get
     * \return the property'double value or -1.0 if not found
     */
    Q_INVOKABLE double getAnimDoubleValue(int frame, QString key);

    /** Get a QRectF associated to the name at a frame position.
     *
     * \param frame the frame number
     * \param key the property to get
     * \return the property'QRectF value or QRectF(0.0, 0.0, 0.0, 0.0) if not found
     */
    Q_INVOKABLE QRectF getAnimRectValue(int frame, QString key);

    /** Remove the keyframe at the specified position.
     *
     * \param frame the frame number in the clip of the animation node to remove
     * \param name the property to remove key frame
     */
    Q_INVOKABLE void removeAnimationKeyFrame(int nFrame, QString name, bool bFromUndo = false);


//#endif

    /** A Boolean value that indicates whether the animation is enable state.
     *
     * \return true when animation is enable; otherwise, false
     */
    Q_INVOKABLE bool enableAnimation() const { return m_bEnableAnimation; }

    /** Sets the animation’s enable state to the value specified by bEnableAnimation.
     *
     * \param bEnableAnimation the animation is enable or not
     */
    Q_INVOKABLE void setEnableAnimation(bool bEnableAnimation) { m_bEnableAnimation = bEnableAnimation; }

    /** A Boolean value that indicates whether the auto add keyframes function is on.
     *
     * \return true when automatically add keyframes; otherwise, false
     */
    Q_INVOKABLE bool autoAddKeyFrame() const { return m_bAutoAddKeyFrame; }

    /** Sets the Auto add keyframes function’s enable state to the value specified by bAutoAddKeyFrame.
     *
     * \param bAutoAddKeyFrame enables the Auto add keyframes function or not
     */
    Q_INVOKABLE void setAutoAddKeyFrame(bool bAutoAddKeyFrame) { m_bAutoAddKeyFrame = bAutoAddKeyFrame; }


    /** sends editKeyframeOfParameter signal
     *
     * \param nIndexOfParameter index of the parameter
     */
    Q_INVOKABLE void emitEditKeyframeOfParameter(const QString strIdentifierOfParameter);


    Q_INVOKABLE bool isKeyframeActivate(const QString strIdentifierOfParameter);
    Q_INVOKABLE bool isKeyframeAtPosition(const QString strIdentifierOfParameter, int nFramePosition);
    Q_INVOKABLE bool hasPreKeyframeAtPositon(const QString strIdentifierOfParameter, int nFramePosition);
    Q_INVOKABLE bool hasNextKeyframeAtPositon(const QString strIdentifierOfParameter, int nFramePosition);

public slots:
    /** Load a properties preset.
     *
     * \param name the absolute file name
     */
    void preset(const QString& name);

signals:
    /// Used to notify the receiver whenever the current presets changed.
    void presetsChanged();

    /** Used to notify the receiver that the analysis is complete.
     *
     * \param isSuccess indicates whether the analysis is successful
     */
    void analyzeFinished(bool isSuccess);

    /// Use to let UI and VUI QML signal updates to each other.
    void filterPropertyValueChanged();

//#ifdef MOVIEMATOR_PRO
    /// Used to notify the receiver whenever new keyframe data inserted into cache.
    void cache_addedKeyFrame();

    /// Used to notify the receiver whenever keyframe data removed from cache.
    void cache_removedKeyFrame();

    /// Used to notify the receiver whenever keyframe cache data changed.Expiration method
    void keyFrameChanged();

    /// Used to notify the receiver whenever the key frame number changed.
    void keyframeNumberChanged();
//#endif

    ///将要编辑参数的关键帧
    void editKeyframeOfParameter(const QString strIdentifierOfParameter);

private:
    QmlMetadata* m_metadata;    /** the metadata of the Mlt::Filter*/
    Mlt::Filter* m_filter;      /** the current Mlt::Filter*/
    QString m_sResourcePath;             /** the absolute path to the metadata directory of the Mlt::Filter*/
    bool m_isNew;               /** a flag to indicate if the filter is new*/
    QStringList m_presets;      /** the preset list of the Mlt::Filter*/
    
    /** Get the object name or mlt_service of Mlt::Filter.
     *
     * \return the object name, the mlt_service if the object name is empty
     */
    QString objectNameOrService();

    /** Get the keyfame at the previous following of the point from cached data.
     *
     * \param currentKeyFrame the frame number at which to start looking for the previous animation node
     * \return the integer keyframe in parent clip, -1 if not found
     */
    int cache_getPreKeyFrameNumInParent(int currentKeyFrame, QString propertyName = nullptr);

    //获取filter中任何一个支持动画的属性名
    QString getAnyAnimPropertyName();

    //QVector<key_frame_item> m_cacheKeyFrameList; /** Cache data for keyframes*/
    bool m_bEnableAnimation;                /** a flag to indicate if the animation is enable state or not*/
    bool m_bAutoAddKeyFrame;                /** a flag to indicate if automatically add key frames or not*/
};

class AnalyzeDelegate : public QObject
{
    Q_OBJECT
public:
    explicit AnalyzeDelegate(Mlt::Filter *filter);

public slots:
    /** Modify the filter properties after analysis is complete.
     * \param task MeltTask was created at analyze
     * \param isSuccess indicates whether the analysis is successful
     */
    void onAnalyzeFinished(AbstractTask *task, bool isSuccess);

private:
    Mlt::Filter m_filter;
};

#endif // FILTER_H
