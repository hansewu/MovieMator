/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: WanYuanCN <ebthon@hotmail.com>
 * Author: Dragon-S <15919917852@163.com>
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

#ifndef EFFECTDOCK_H
#define EFFECTDOCK_H

#include "effectdock_global.h"

#include <QDir>
#include <QLabel>
#include <QMimeData>
#include <QDockWidget>
#include <QSpacerItem>
#include <maininterface.h>
#include <QJsonObject>

#include "effectdockinterface.h"
#include "effectlistmodel.h"
#include "effectlistview.h"

namespace Ui {
    class EffectDock;
}

class EffectDock : public QDockWidget
{
    Q_OBJECT

public:
    explicit EffectDock(MainInterface *main=nullptr, QWidget *parent=nullptr);
    ~EffectDock();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    // 图片添加特效
    void replaceImage(QString effectFile, QString imageFile);
    void setMimeDataForDrag();
    void createEffectFile();

    void createImageFileList(QFileInfoList &fileList, QString folderName);
    void appendListViewAndLabel(EffectListModel *model, QString itemName);
    void readJsonFile(QString filePath, QJsonObject &jsonObj);
    QString getTranslationStr(QString srcStr, QJsonObject translationInfo);

private slots:
    void on_listView_pressed(const QModelIndex&);
    void on_listView_clicked(const QModelIndex&);
    void on_listView_customContextMenuRequested(const QPoint&);
    void on_actionAddToTimeline_triggered();
    void on_comboBox_activated(int index);
    void on_comboBox_currentIndexChanged(int index);
    void on_comboBox_2_currentIndexChanged(int index);
    void on_EffectDock_visibilityChanged(bool visible);

private:
    Ui::EffectDock *ui;
    MainInterface *m_mainWindow;

    QDir m_dir;
    FILE_HANDLE m_effectFile;
    QList<QString> *m_effectList;
    QMimeData *m_mimeData;

    QList<EffectListView*> *m_imageList;
    QModelIndex m_currentIndex;
    EffectListView *m_currentListView;

    QSpacerItem *m_spacerItem;

    QJsonObject m_animationNameTranslateInfo;
    QJsonObject m_imageClassNameTranslateInfo;
};

#endif // EFFECTDOCK_H
