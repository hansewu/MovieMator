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
#ifndef ABSTRACTTASK_H
#define ABSTRACTTASK_H

#include <QObject>
#include <QModelIndex>

class AbstractTask : public QObject
{
    Q_OBJECT
public:
    explicit AbstractTask(const QString &name);


    void setModelIndex(const QModelIndex &index);
    QModelIndex modelIndex () const;

    void setLabel(const QString& name);
    QString label() {return m_label;}


    bool ran() {return m_ran;}
    bool stopped() {return m_stopped;}
    bool finishedNormally() {return m_finishedNormally;}
    bool running() {return m_running; }
    bool killed() {return m_killed;}
    void setStopped(bool stopped);
    void setFinishedNormally(bool finishedNormally);

signals:
    void progressUpdated(QModelIndex index, uint percent);
    void finished(AbstractTask *task, bool isSuccess);

public slots:
    virtual void start();
    virtual void stop();

protected:
    QModelIndex     m_index;//通过m_index与EncodeTaskQueue 里面的Item连接起来

private:
    bool            m_ran; //已经运行过，可能正在运行，也可能已经停止运行
    bool            m_running; //正在运行
    bool            m_finishedNormally; //执行完，正常结束
    bool            m_killed; //强制stop
    bool            m_stopped;//停止状态(可能正常执行完成，也可能是killed)
    QString         m_label;

};

#endif // ABSTRACTTASK_H
