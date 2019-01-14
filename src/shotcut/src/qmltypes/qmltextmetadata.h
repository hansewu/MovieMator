/*
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
