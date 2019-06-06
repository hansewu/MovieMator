/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl1987527 <wyl1987527@163.com>
 * Author: Author: fuyunhuaxin <2446010587@qq.com>
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

#include "AudioFilterdock.h"
#include <QQuickWidget>
#include <QQuickView>
#include <QQmlContext>
#include <AudioFilterdockinterface.h>
#include <QDir>
/*
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: wyl1987527 <wyl1987527@163.com>
 * Author: Author: fuyunhuaxin <2446010587@qq.com>
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

#include <QStandardPaths>
#include <QtQml>
#include <qmlutilities.h>
#include <qmlview.h>

AudioFilterDock::AudioFilterDock(MainInterface *main, QWidget *parent):
    QDockWidget(parent),
  m_qview(QmlUtilities::sharedEngine(), this)
{
    m_mainWindow = main;
    m_pAudioFilterInfo = nullptr;
    m_qview.setFocusPolicy(Qt::StrongFocus);
    setWidget(&m_qview);

    QmlUtilities::setCommonProperties(m_qview.rootContext());

    createAudioFilterDockPage();
}

AudioFilterDock::~AudioFilterDock()
{
    if(m_pAudioFilterInfo) {delete m_pAudioFilterInfo; m_pAudioFilterInfo = nullptr;}
}

void AudioFilterDock::resetQview()
{
//    Q_ASSERT(&m_qview);
//    if(!(&m_qview)) return;
    QDir viewPath = QmlUtilities::qmlDir();
    viewPath.cd("views");
    viewPath.cd("filter");
    m_qview.engine()->addImportPath(viewPath.path());

    QDir modulePath = QmlUtilities::qmlDir();
    modulePath.cd("modules");
    m_qview.engine()->addImportPath(modulePath.path());

    m_qview.setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qview.quickWindow()->setColor(palette().window().color());
    QUrl source = QUrl::fromLocalFile(viewPath.absoluteFilePath("AudioFiltersUI.qml"));
    m_qview.setSource(source);
}

int AudioFilterDock::createAudioFilterDockPage()
{
//    Q_ASSERT(&m_qview);
//    if(!(&m_qview)) return 1;
    qmlRegisterType<AudioFilterItemInfo>("AudioFilterItemInfo", 1, 0, "AudioFilterItemInfo");

    m_qview.rootContext()->setContextProperty("AudioFiltersInfo", m_pAudioFilterInfo);
    m_qview.rootContext()->setContextProperty("AudioFiltersResDock", this);

    resetQview();
    return 0;
}

int AudioFilterDock::updateAudioFilters(AudioFilter_Info * AudioFilterInfos, int nAudioFilterCount)
{

    Q_ASSERT(AudioFilterInfos);
    if(!AudioFilterInfos) return 1;
//    Q_ASSERT(&m_qview);
//    if(!(&m_qview)) return 1;

    if(m_pAudioFilterInfo) {delete m_pAudioFilterInfo; m_pAudioFilterInfo = nullptr;}

    m_pAudioFilterInfo = new AudioFiltersInfo();
    for(int i = 0; i < nAudioFilterCount; i++)
    {
        AudioFilterItemInfo *AudioFilterInfo = new AudioFilterItemInfo();

        AudioFilterInfo->setVisible(AudioFilterInfos[i].visible);
        AudioFilterInfo->setName(QString(AudioFilterInfos[i].name));
        AudioFilterInfo->setAudioFilterType(QString(AudioFilterInfos[i].type));
        AudioFilterInfo->setImageSourcePath(QString(AudioFilterInfos[i].imageSourcePath));

        m_pAudioFilterInfo->addAudioFilterItemInfo(AudioFilterInfo);
    }

    return 0;
}

void AudioFilterDock::addAudioFilterItem(int index)
{
    m_mainWindow->addFilter(index);
}


void AudioFiltersInfo::addAudioFilterItemInfo(AudioFilterItemInfo *AudioFilterInfo)
{
//    Q_ASSERT(&m_AudioFilterInfoList);
//    if(!(&m_AudioFilterInfoList)) return;
    Q_ASSERT(AudioFilterInfo);
    if(!AudioFilterInfo) return;
    m_AudioFilterInfoList.append(AudioFilterInfo);
}


static AudioFilterDock *ftDocInstance = nullptr;
QDockWidget *AudioFilterDock_initModule(MainInterface *main)
{
    if (ftDocInstance == nullptr)
        ftDocInstance = new AudioFilterDock(main);

    return ftDocInstance;
}

void AudioFilterDock_destroyModule()
{
    if(ftDocInstance) delete ftDocInstance;
}

int setAudioFiltersInfo(AudioFilter_Info * AudioFilterInfos, int nAudioFilterCount)
{
    Q_ASSERT(ftDocInstance);
    if(!ftDocInstance) return 1;
    Q_ASSERT(AudioFilterInfos);
    if(!AudioFilterInfos) return 1;
    ftDocInstance->updateAudioFilters(AudioFilterInfos, nAudioFilterCount);
    ftDocInstance->createAudioFilterDockPage();

    return 0;
}

int getCurrentSelectedAudioFilterIndex()
{
    return 0;
}
