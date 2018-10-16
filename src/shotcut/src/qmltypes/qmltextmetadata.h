#ifndef QMLTEXTMETADATA_H
#define QMLTEXTMETADATA_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QUrl>

class QmlTextMetaData : public QObject
{
    Q_OBJECT


    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString mlt_service READ mlt_service WRITE set_mlt_service)
    Q_PROPERTY(QString thumbnail READ thumbnail WRITE setThumbnail)

public:
    explicit QmlTextMetaData(QObject *parent = 0);


    QString name() const { return m_name; }
    void setName(const QString&);
    QString mlt_service() const { return m_mlt_service; }
    void set_mlt_service(const QString&);

    QString thumbnail() const { return m_thumbnail; }
    void setThumbnail(const QString &);

signals:
    void changed();

private:

    QString m_name;
    QString m_mlt_service;

    QString m_thumbnail;
};

#endif // QMLTEXTMETADATA_H
