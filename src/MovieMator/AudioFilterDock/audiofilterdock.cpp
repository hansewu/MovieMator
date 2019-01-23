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
    m_pAudioFilterInfo = NULL;
    m_qview.setFocusPolicy(Qt::StrongFocus);
    setWidget(&m_qview);

    QmlUtilities::setCommonProperties(m_qview.rootContext());

    updateAudioFilters(NULL, 0);
}

AudioFilterDock::~AudioFilterDock()
{
    if(m_pAudioFilterInfo) {delete m_pAudioFilterInfo; m_pAudioFilterInfo = NULL;}
}

void AudioFilterDock::resetQview()
{
    QDir viewPath = QmlUtilities::qmlDir();
    viewPath.cd("views");
    viewPath.cd("AudioFilter");
    m_qview.engine()->addImportPath(viewPath.path());

    QDir modulePath = QmlUtilities::qmlDir();
    modulePath.cd("modules");
    m_qview.engine()->addImportPath(modulePath.path());

    m_qview.setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qview.quickWindow()->setColor(palette().window().color());
    QUrl source = QUrl::fromLocalFile(viewPath.absoluteFilePath("AudioFiltersUI.qml"));
    m_qview.setSource(source);
}

int AudioFilterDock::updateAudioFilters(AudioFilter_Info * AudioFilterInfos, int nAudioFilterCount)
{
    if(m_pAudioFilterInfo) {delete m_pAudioFilterInfo; m_pAudioFilterInfo = NULL;}

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

    qmlRegisterType<AudioFilterItemInfo>("AudioFilterItemInfo", 1, 0, "AudioFilterItemInfo");

    m_qview.rootContext()->setContextProperty("AudioFiltersInfo", m_pAudioFilterInfo);
    m_qview.rootContext()->setContextProperty("AudioFiltersResDock", this);

    resetQview();

    return 0;
}

void AudioFilterDock::addAudioFilterItem(int index)
{
    m_mainWindow->addFilter(index);
}


void AudioFiltersInfo::addAudioFilterItemInfo(AudioFilterItemInfo *AudioFilterInfo)
{
    m_AudioFilterInfoList.append(AudioFilterInfo);
}


static AudioFilterDock *ftDocInstance = 0;
QDockWidget *AudioFilterDock_initModule(MainInterface *main)
{
    if (ftDocInstance == NULL)
        ftDocInstance = new AudioFilterDock(main);

    return ftDocInstance;
}

void AudioFilterDock_destroyModule()
{
    if(ftDocInstance) delete ftDocInstance;
}

int setAudioFiltersInfo(AudioFilter_Info * AudioFilterInfos, int nAudioFilterCount)
{
    ftDocInstance->updateAudioFilters(AudioFilterInfos, nAudioFilterCount);

    return 0;
}

int getCurrentSelectedAudioFilterIndex()
{
    return 0;
}
