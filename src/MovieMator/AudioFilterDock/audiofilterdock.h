#ifndef AUDIOAUDIOFILTERDOCK_H
#define AUDIOAUDIOFILTERDOCK_H

#include <QDockWidget>
#include <QQuickWidget>
#include <maininterface.h>
#include "audiofilterdock_global.h"
#include "audiofilterdockinterface.h"


class AudioFilterItemInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString AudioFilterType READ AudioFilterType WRITE setAudioFilterType)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString imageSourcePath READ imageSourcePath WRITE setImageSourcePath)
    Q_PROPERTY(bool visible READ visible WRITE setVisible)

public:

    explicit AudioFilterItemInfo(QObject *parent = NULL){}
    ~AudioFilterItemInfo(){};

    Q_INVOKABLE QString AudioFilterType() const { return m_AudioFilterType; }
    Q_INVOKABLE void setAudioFilterType(const QString strType){m_AudioFilterType = strType;}
    Q_INVOKABLE QString name() const { return m_name; }
    Q_INVOKABLE void setName(const QString strName){m_name = strName;}
    Q_INVOKABLE QString imageSourcePath() const { return m_imageSourcePath; }
    Q_INVOKABLE void setImageSourcePath(const QString strImageSourcePath){m_imageSourcePath = strImageSourcePath;}
    Q_INVOKABLE bool visible() const { return m_bVisible; }
    Q_INVOKABLE void setVisible(const bool visible){m_bVisible = visible;}

private:
    QString m_AudioFilterType;
    QString m_name;
    QString m_imageSourcePath;
    bool m_bVisible;
};

class AudioFiltersInfo: public QObject
{
    Q_OBJECT
public:
    AudioFiltersInfo(){m_AudioFilterInfoList.clear();}
    ~AudioFiltersInfo(){}

    QVariant data(const QModelIndex &index, int role) const;

    Q_INVOKABLE int rowCount(){return m_AudioFilterInfoList.size();}
    Q_INVOKABLE AudioFilterItemInfo* get(int index) const {return m_AudioFilterInfoList.at(index);}

    void addAudioFilterItemInfo(AudioFilterItemInfo* AudioFilterInfo);

private:
    typedef QList<AudioFilterItemInfo*> AudioFiltersList;
    AudioFiltersList m_AudioFilterInfoList;
};

class AUDIOFILTERDOCKSHARED_EXPORT AudioFilterDock: public QDockWidget
{
    Q_OBJECT

public:
    explicit AudioFilterDock(MainInterface *main=0, QWidget *parent = 0);
    ~AudioFilterDock();

    int updateAudioFilters(AudioFilter_Info * AudioFilterInfos, int nAudioFilterCount);
    void resetQview();

    Q_INVOKABLE void addAudioFilterItem(int index);

private:
    MainInterface *m_mainWindow;
    QQuickWidget m_qview;
    AudioFiltersInfo *m_pAudioFilterInfo;
};



#endif // AUDIOAudioFilterDOCK_H
