#include "qmltextmetadata.h"

QmlTextMetaData::QmlTextMetaData(QObject *parent) : QObject(parent)
{

}


void QmlTextMetaData::setName(const QString &name)
{
    m_name = name;
}

void QmlTextMetaData::set_mlt_service(const QString &service)
{
    m_mlt_service = service;
}


void QmlTextMetaData::setThumbnail(const QString &thumbnail)
{
    m_thumbnail = thumbnail;
}
