/*
 * Copyright (c) 2013-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
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

#ifndef QMLMETADATA_H
#define QMLMETADATA_H

#include "qmlutilities_global.h"

#include <QObject>
#include <QString>
#include <QDir>
#include <QUrl>
#include <QVector>
#include <QMap>
#include <QQmlListProperty>

/*!
  \class QmlKeyframesParameter

  QmlKeyframesParameter is a wrapper around the metadata for a keyframe parameter of the filter.
  QmlKeyframesParameter provides access to the parameter propertied and data.

*/
class QMLUTILITIESSHARED_EXPORT QmlKeyframesParameter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name NOTIFY changed)
    Q_PROPERTY(QString property MEMBER m_property NOTIFY changed)
    /// If isSimple this parameter cannot use full keyframes while simple is in use.
    Q_PROPERTY(bool isSimple MEMBER m_isSimple NOTIFY changed)
    Q_PROPERTY(bool isCurve MEMBER m_isCurve NOTIFY changed)
    Q_PROPERTY(double minimum MEMBER m_minimum NOTIFY changed)
    Q_PROPERTY(double maximum MEMBER m_maximum NOTIFY changed)

    Q_PROPERTY(QString explanation MEMBER m_explanation NOTIFY changed)
    Q_PROPERTY(QString objectName MEMBER m_objectName NOTIFY changed)
    Q_PROPERTY(QString controlType MEMBER m_controlType NOTIFY changed)
    Q_PROPERTY(QString paraType MEMBER m_paraType NOTIFY changed)
    Q_PROPERTY(QString defaultValue MEMBER m_defaultValue NOTIFY changed)
    Q_PROPERTY(QString value MEMBER m_value NOTIFY changed)
    Q_PROPERTY(QList<QString> factorFunc MEMBER m_factorFunc NOTIFY changed)

public:
    explicit QmlKeyframesParameter(QObject* parent = 0);

    /** Get the parameter display name, used for displaying on the interface.
     *
     * \return a QString value
     */
    QString name() const { return m_name; }

    /** Set the parameter display name, used for displaying on the interface.
     *
     * \param name the property display name
     */
    void setName(const QString &name) {m_name = name;};

    /** Get the property name of the parameter.
     *
     * \return a QString value
     */
    QString property() const { return m_property; }

    /** Set the property name of the parameter.
     *
     * \param name the property name
     */
    void setProperty(const QString &property) {m_property = property;};

    /** did not use*/
    bool isSimple() const { return m_isSimple; }
    /** did not use*/
    void setIsSimple(const bool isSimple) {m_isSimple = isSimple;};
    /** did not use*/
    bool isCurve() const { return m_isCurve; }
    /** did not use*/
    void setIsCurve(const bool isCurve) {m_isCurve = isCurve;};

    /** Get the minimum value for the parameter.
     *
     * \return a double value
     */
    double minimum() const { return m_minimum; }

    /** Set a minimum value for the parameter.
     *
     * \param minimum the minimum value
     */
    void setMinimum(const double minimum) {m_minimum = minimum;};

    /** Get the maximum value for the parameter.
     *
     * \return a double value
     */
    double maximum() const { return m_maximum; }

    /** Set a maximum value for the parameter.
     *
     * \param maximum the minimum value
     */
    void setMaximum(const double maximum) {m_maximum = maximum;};

    /** Get the explanation string of the parameter.
     *
     * \return an explanation string
     */
    QString explanation() const { return m_explanation; }

    /** Set an explanation string for the parameter.
     *
     * \param explanation an explanation string
     */
    void setExplanation(const QString &explanation) {m_explanation = explanation;};

    /** Get the object name of the control that displays the parameter.
     *
     * \return a QString string
     */
    QString objectName() const { return m_objectName; }

    /** Set the object name of the control that displays the parameter.
     *
     * \param objectName a QString string
     */
    void setObjectName(const QString &objectName) {m_objectName = objectName;};

    /** Get the type of the control that displays the parameter.
     *
     * \return a QString string
     */
    QString controlType() const { return m_controlType; }

    /** Set the type of the control that displays the parameter.
     *
     * \param controlType a QString string
     */
    void setControlType(const QString &controlType) {m_controlType = controlType;};

    /** Get the type of the parameter, int,string,rect,double.
     *
     * \return a QString string
     */
    QString paraType() const { return m_paraType; }

    /** Set the type of the parameter, int,string,rect,double.
     *
     * \param paraType a QString string
     */
    void setParaType(const QString &paraType) {m_paraType = paraType;};

    /** Get the default value for the parameter.
     *
     * \return a QString value
     */
    QString defaultValue() const { return m_defaultValue; }

    /** Set a default value for parameter.
     *
     * \param defaultValue the default value
     */
    void setDefaultValue(const QString &defaultValue) {m_defaultValue = defaultValue;};

    /** Get the value for the parameter.
     *
     * \return a QString value
     */
    QString value() const { return m_value; }

    /** Set a value for parameter.
     *
     * \param value the value
     */
    void setValue(const QString &value) {m_value = value;};

    /** Get the formula for calculating the parameter's interface value.
     *
     * \return a QList<QString> value
     */
    QList<QString> factorFunc() const { return m_factorFunc; }

    /** Set the formula for calculating the parameter‘s interface value.
     *
     * \param factorFunc the formula for calculating the property‘s interface value
     */
    void setFactorFunc(const QList<QString> &factorFunc) {m_factorFunc.append(factorFunc);};
signals:
    void changed();   /** did not use*/

private:
    QString m_name;         /** the display name of the parameter*/
    QString m_property;     /** the property name of the parameter*/
    bool m_isSimple;        /** did not use*/
    bool m_isCurve;         /** did not use*/
    double m_minimum;       /** the minimum value of the parameter*/
    double m_maximum;       /** the maximum value of the parameter*/

    QString m_explanation;  /** An explanation string of the parameter*/
    QString m_objectName;   /** the object name of the control that displays the parameter*/
    QString m_controlType;  /** the type of the control that displays the parameter*/
    QString m_paraType;     /** the type of the parameter, int,string,rect,double*/
    QString m_defaultValue; /** the default value of the parameter*/
    QString m_value;        /** the value of the parameter*/
    QList<QString> m_factorFunc;    /** the formula for calculating the parameter's interface value*/
};

/*!
  \class QmlKeyframesMetadata

  QmlKeyframesMetadata is a wrapper around the metadata for all keyframe parameters of the filter.
  QmlKeyframesMetadata provides access to the keyframe propertied and data.

*/
class QMLUTILITIESSHARED_EXPORT QmlKeyframesMetadata : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool allowTrim MEMBER m_allowTrim NOTIFY changed)
    Q_PROPERTY(bool allowAnimateIn MEMBER m_allowAnimateIn NOTIFY changed)
    Q_PROPERTY(bool allowAnimateOut MEMBER m_allowAnimateOut NOTIFY changed)
    Q_PROPERTY(QQmlListProperty<QmlKeyframesParameter> parameters READ parameters NOTIFY changed)
    /// simpleProperties identifies a list of properties whose keyframe position must be updated when trimming.
    Q_PROPERTY(QList<QString> simpleProperties MEMBER m_simpleProperties NOTIFY changed)
    Q_PROPERTY(QString minimumVersion MEMBER m_minimumVersion NOTIFY changed)
    Q_PROPERTY(bool enabled MEMBER m_enabled NOTIFY changed)
    Q_PROPERTY(int parameterCount READ parameterCount NOTIFY changed)

public:
    explicit QmlKeyframesMetadata(QObject *parent = 0);

    bool allowTrim() const { return m_allowTrim; }              /** did not use*/

    /** A Boolean value that indicates whether the animation can support in. did not use.
     *
     * \return true the animation can support in, otherwise, false
     */
    bool allowAnimateIn() const { return m_allowAnimateIn; }

    /** A Boolean value that indicates whether the animation can support out. did not use.
     *
     * \return true the animation can support out, otherwise, false
     */
    bool allowAnimateOut() const { return m_allowAnimateOut; }

    /** Get the properties of the filter.
     *
     * \return a QList<QString> value
     */
    QList<QString> simpleProperties() const { return m_simpleProperties; }

//    QQmlListProperty<QmlKeyframesParameter> parameters() { return QQmlListProperty<QmlKeyframesParameter>(this, m_parameters); }
    /** Get the parameters of the filter, QmlKeyframesParameter.
     * the parameter is QmlKeyframesParameter object
     * \return a QQmlListProperty<QmlKeyframesParameter> value
     */
    QQmlListProperty<QmlKeyframesParameter> parameters();

    /** Get the paramter number of the filter.
     *
     * \return an integer value
     */
    int parameterCount() const { return m_parameters.count(); }

    /** Get the keyframes parameter info for the N-th parameter.
     *
     * \param index the N-th parameter (0 based)
     * \return The QmlKeyframesParameter value, the parameter info
     */
    QmlKeyframesParameter *parameter(int index) const { return m_parameters[index]; }

    /** Check the version number. did not use
     * If the version number is less than the minimum supported version number, the animation function can not support the in and out.
     * \param version the current version
     */
    void checkVersion(const QString& version);

    /** Disable animation in and out. did not use
     * If the version number is less than the minimum supported version number, the animation function can not support the in and out.
     */
    void setDisabled();

    /** Append a parameter to the end of the keyframe parameter list.
     * the type of the parameter is QmlKeyframesParameter
     * \param param will be added to the end of the keyframe parameter list
     */
    void appendParameter(QmlKeyframesParameter * param);

    /** Get the paramter number of the filter.
     *
     * \return an integer value
     */
    int paramCount() const;

    /** Get the keyframes parameter info for the N-th parameter.
     *
     * \param index the N-th parameter (0 based)
     * \return The QmlKeyframesParameter value, the parameter info
     */
    QmlKeyframesParameter *paramAt(int idx) const;

    /** Clear the filter's parameter list.
     *
     */
    void clearParameter();

signals:
    void changed();             /** did not use*/

private:
    bool m_allowTrim;           /** did not use*/
    bool m_allowAnimateIn;      /** A Boolean value that indicates whether the animation can support in*/
    bool m_allowAnimateOut;     /** A Boolean value that indicates whether the animation can support out*/
    QList<QmlKeyframesParameter *> m_parameters;    /** the parameters of the filter*/
    QList<QString> m_simpleProperties;              /** the properties of the filter*/
    QString m_minimumVersion;                       /** the minumum version, the animation can support in and out */
    bool m_enabled;             /** did not use*/

    /** Append a parameter to the end of the parameter list.
     * the type of the parameter is QmlKeyframesParameter
     * \param paramsList the parameter list
     * \param param will be added to the end of the parameter list
     */
    static void appendParameter(QQmlListProperty<QmlKeyframesParameter> *paramsList, QmlKeyframesParameter * param);

    /** Get the paramter list number.
     *
     * \param paramsList the parameter list
     * \return an integer value
     */
    static int paramCount(QQmlListProperty<QmlKeyframesParameter> *);

    /** Get the parameter info for the N-th parameter.
     *
     * \param idx the N-th parameter (0 based)
     * \return The QmlKeyframesParameter value, the parameter info
     */
    static QmlKeyframesParameter *paramAt(QQmlListProperty<QmlKeyframesParameter> *,int idx);

    /** Clear the parameter list.
     *
     */
    static void clearParameter(QQmlListProperty<QmlKeyframesParameter> *);

};
//struct key_frame_item
//{
//   double keyFrame;
//   QMap<QString, QString> paraMap;
//};

class QMLUTILITIESSHARED_EXPORT QmlMetadata : public QObject
{
    Q_OBJECT
    Q_ENUMS(PluginType)
    Q_PROPERTY(PluginType type READ type WRITE setType)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString mlt_service READ mlt_service WRITE set_mlt_service)
    Q_PROPERTY(bool needsGPU READ needsGPU WRITE setNeedsGPU NOTIFY changed)
    Q_PROPERTY(QString qml READ qmlFileName WRITE setQmlFileName)
    Q_PROPERTY(QString vui READ vuiFileName WRITE setVuiFileName)
    Q_PROPERTY(QUrl qmlFilePath READ qmlFilePath )
    Q_PROPERTY(QUrl vuiFilePath READ vuiFilePath )
    Q_PROPERTY(bool isAudio READ isAudio WRITE setIsAudio NOTIFY changed)
    Q_PROPERTY(bool isHidden READ isHidden WRITE setIsHidden NOTIFY changed)
    Q_PROPERTY(bool isFavorite READ isFavorite WRITE setIsFavorite NOTIFY changed)
    Q_PROPERTY(QString gpuAlt READ gpuAlt WRITE setGpuAlt NOTIFY changed)
    Q_PROPERTY(bool allowMultiple READ allowMultiple WRITE setAllowMultiple)
    Q_PROPERTY(bool isClipOnly READ isClipOnly WRITE setIsClipOnly)
    Q_PROPERTY(QString thumbnail READ thumbnail WRITE setThumbnail)
    Q_PROPERTY(bool needsProVersion READ needsProVersion WRITE setNeedsProVersion)
    Q_PROPERTY(bool freeVersion READ freeVersion WRITE setFreeVersion)
    Q_PROPERTY(bool isGpuCompatible READ isGpuCompatible() WRITE setIsGpuCompatible)
    Q_PROPERTY(QmlKeyframesMetadata* keyframes READ keyframes NOTIFY changed)
    Q_PROPERTY(QString filterType READ filterType WRITE setFilterType)


public:
    enum PluginType {
        Filter,
        Producer,
        Transition
    };

    explicit QmlMetadata(QObject *parent = 0);
    void loadSettings();

    PluginType type() const { return m_type; }
    void setType(PluginType);
    QString name() const { return m_name; }
    void setName(const QString&);
    QString mlt_service() const { return m_mlt_service; }
    void set_mlt_service(const QString&);
    QString uniqueId() const;
    bool needsGPU() const { return m_needsGPU; }
    void setNeedsGPU(bool);
    QString qmlFileName() const { return m_qmlFileName; }
    void setQmlFileName(const QString&);
    QString vuiFileName() const { return m_vuiFileName; }
    void setVuiFileName(const QString&);
    QDir path() const { return m_path; }
    void setPath(const QDir& path);
    QUrl qmlFilePath() const;
    QUrl vuiFilePath() const;
    bool isAudio() const { return m_isAudio; }
    void setIsAudio(bool isAudio);
    bool isHidden() const { return m_isHidden; }
    void setIsHidden(bool isHidden);
    bool isFavorite() const { return m_isFavorite; }
    void setIsFavorite(bool isFavorite);
    QString gpuAlt() const { return m_gpuAlt; }
    void setGpuAlt(const QString&);
    bool allowMultiple() const { return m_allowMultiple; }
    void setAllowMultiple(bool allowMultiple);
    bool isClipOnly() const { return m_isClipOnly; }
    void setIsClipOnly(bool isClipOnly);
    QString thumbnail() const { return m_thumbnail; }
    void setThumbnail(const QString &);
    void setNeedsProVersion(bool needsProVersion);
    bool needsProVersion() const { return m_needsProVersion; }
    void setFreeVersion(bool freeVersion);
    bool freeVersion() const { return m_freeVersion; }
    bool isGpuCompatible() const { return m_isGpuCompatible; }
    void setIsGpuCompatible(bool isCompatible) { m_isGpuCompatible = isCompatible; }
    QmlKeyframesMetadata* keyframes() { return &m_keyframes; }

    QString filterType() const { return m_filterType; }
    void setFilterType(const QString&);

signals:
    void changed();

private:
    PluginType m_type;

    QString m_name;
    QString m_mlt_service;
    bool m_needsGPU;
    QString m_qmlFileName;
    QString m_vuiFileName;
    QDir m_path;
    bool m_isAudio;
    bool m_isHidden;
    bool m_isFavorite;
    QString m_gpuAlt;
    bool m_allowMultiple;
    bool m_isClipOnly;
    QString m_thumbnail;
    bool m_needsProVersion;
    bool m_freeVersion;
    bool m_isGpuCompatible;
    QmlKeyframesMetadata m_keyframes;
//   QVector<key_frame_item> m_keyFrameList;
    QString m_filterType;
};

#endif // QMLMETADATA_H
