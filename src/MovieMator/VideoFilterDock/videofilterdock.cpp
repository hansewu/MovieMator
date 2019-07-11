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

#include "videofilterdock.h"
#include "ui_videofilterdock.h"
#include "settings.h"
#include "util.h"

#include <QMenu>
#include <Logger.h>
#include <QScrollBar>
#include <QFile>
#include <QDebug>
#include <QImage>

VideoFilterDock::VideoFilterDock(int filterDockType, MainInterface *main, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::VideoFilterDock),
    m_mainWindow(main),
    m_filterDockType(filterDockType)
{
    LOG_DEBUG() << "begin";

    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());

    m_filterList = new QList<VideoFilterListView*>;
    m_currentListView = nullptr;

    if (filterDockType == 0) {//视频滤镜
        ui->dockName->setText(tr("Video Filter"));
    } else if (filterDockType == 1){//音频滤镜
        ui->comboBox_2->setHidden(true);//音频滤镜目前只有一个分类，暂时隐藏分类控件
        ui->dockName->setText(tr("Audio Filter"));
    }
    ui->comboBox_2->setStyleSheet("QComboBox { background-color:rgb(100,100,100);color:rgb(225,225,225); }");

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setStyleSheet("border:none;");
    ui->scrollArea->verticalScrollBar()
            ->setStyleSheet("QScrollBar:vertical{width:8px;background:rgba(0,0,0,0%);margin:0px,0px,0px,0px;}"
                            "QScrollBar::handle:vertical{width:8px;background:rgba(160,160,160,25%);border-radius:4px;min-height:20;}"
                            "QScrollBar::handle:vertical:hover{width:8px;background:rgba(160,160,160,50%);border-radius:4px;min-height:20;}");

    // 添加到时间线的按钮，整个界面只有一个，而不是为每个 item都安置一个
    m_addButton = new AddButton(this);  // QIcon(":/icons/light/32x32/filter_add.png")
    m_addButton->setStyleSheet("QPushButton{ border-image: url(:/icons/light/32x32/filter_add.png)}"
                               "QPushButton:pressed{ border-image: url(:/icons/light/32x32/filter_add-a.png)}");
    m_addButton->setFixedSize(QSize(27, 26));
    m_addButton->setVisible(false);
    connect(m_addButton, SIGNAL(clicked()), this, SLOT(addToTimeline()));
    connect(m_addButton, SIGNAL(mouseEnter()), this, SLOT(setSelection()));
    connect(m_addButton, SIGNAL(mouseLeave()), this, SLOT(resetSelection()));

    LOG_DEBUG() << "end";
}

VideoFilterDock::~VideoFilterDock()
{
    qDeleteAll(*m_filterList);
    m_filterList->clear();
    delete m_filterList;
    m_filterList = nullptr;

    delete ui;
}

void VideoFilterDock::resizeEvent(QResizeEvent *event)
{
    Q_ASSERT(m_filterList);
    if(!m_filterList)
    {
        return;
    }
    for(VideoFilterListView *listView : *m_filterList)
    {
        Q_ASSERT(listView);
        if(!listView)
        {
            continue;
        }
        listView->setFixedWidth(ui->scrollArea->width() -5);
        int wSize = listView->gridSize().width();
        if(wSize<=0)
        {
            continue;
        }
        int hSize = listView->gridSize().height();
        int width = listView->size().width();
        int columns = width/wSize<1 ? 1: width/wSize;
        int rowCount = listView->model()->rowCount();
        int rows = rowCount%columns>0 ? (rowCount/columns+1) : (rowCount/columns);
        listView->setFixedHeight(rows*hSize);

        listView->setColumnCount(columns);
    }
    on_comboBox_2_activated(ui->comboBox_2->currentIndex());
    QDockWidget::resizeEvent(event);
}

// 调整 m_addToTimelineButton的位置，点击时，调整界面大小时
// 位置随当前选中的 item变化而变化
// 鼠标悬浮时显示添加按钮
void VideoFilterDock::positionAddButton()
{
    if(!sender())
        return;

    VideoFilterListView *listView = qobject_cast<VideoFilterListView*>(sender());
    if(!listView->getModelIndex().isValid())
        return;

    int columnCount = listView->getColumnCount();

    int count = listView->getModelIndex().row() + 1;
    int row = (count % columnCount > 0) ? (count / columnCount + 1) : (count / columnCount);
    int column = count - (row-1)*columnCount;

    int gridWidth = listView->gridSize().width()
                    + listView->contentsMargins().left()       // 0
                    + listView->contentsMargins().right();     // 0
    int gridHeight = listView->gridSize().height()
                    + listView->contentsMargins().top()        // 5
                    + listView->contentsMargins().bottom();    // 5

    int width = m_addButton->width();
    int height = m_addButton->height();

    int x = column * gridWidth - width -5;
    int y = (row-1) * gridHeight;

    m_addButton->setParent(listView);
    m_addButton->setVisible(true);
    m_addButton->setGeometry(x, y, width, height);
}

void VideoFilterDock::hideAddButton()
{
    m_addButton->setVisible(false);
}

// 鼠标进入按钮时设置 listView的 item为选中状态
// 通过设置鼠标进入按钮时设置 item为选中状态来维持 item的背景色
// 鼠标进入按钮后，item的悬浮效果会被清除
void VideoFilterDock::setSelection()
{
    if(!sender() || !sender()->parent())
    {
        return;
    }

    VideoFilterListView *listView = qobject_cast<VideoFilterListView*>(sender()->parent());
    listView->selectionModel()->select(listView->getModelIndex(), QItemSelectionModel::Select);
}

// 鼠标离开按钮时重置 listView的 item选中状态
void VideoFilterDock::resetSelection()
{
    if(!sender() || !sender()->parent())
    {
        return;
    }

    VideoFilterListView *listView = qobject_cast<VideoFilterListView*>(sender()->parent());
    listView->clearSelection();
    // 当前 lsitView的当前 item依然要处于选中状态
    if(listView==m_currentListView)
    {
        listView->selectionModel()->select(m_currentIndex, QItemSelectionModel::Select);
    }
}

void VideoFilterDock::addToTimeline()
{
    if(!sender() || !sender()->parent())
        return;

    VideoFilterListView *tmpListView = m_currentListView;       // 保存值
    QModelIndex tmpIndex = m_currentIndex;                 // 保存值

    VideoFilterListView *listView = qobject_cast<VideoFilterListView*>(sender()->parent());
    m_currentListView = listView;
    if(!m_currentListView)
    {
        m_currentListView = tmpListView;
        return;
    }
    m_currentIndex = listView->getModelIndex();
    if(!m_currentIndex.isValid())
    {
        m_currentListView = tmpListView;
        m_currentIndex = tmpIndex;
        return;
    }

    VideoFilterListVideoModel *model = qobject_cast<VideoFilterListVideoModel *>(listView->model());
    Q_ASSERT(model);
    if(model) {
        FilterItemInfo *filterItemInfo = model->fileAt(m_currentIndex.row());
        int filterIndex = filterItemInfo->filterIndex();

        addFilter(filterIndex);
    }
}

void VideoFilterDock::appendListViewAndLabel(VideoFilterListVideoModel *model, QString itemName)
{
    Q_ASSERT(model);
    Q_ASSERT(m_filterList);
    if(!model || !m_filterList)
    {
        return;
    }
    VideoFilterListView *listView = new VideoFilterListView();
    if (m_filterDockType == 0) {
        QLabel *label = new QLabel(itemName, this);
        label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        QLabel *image = new QLabel(this);
        image->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        image->setScaledContents(true);     // 可以让图片随 label拉伸
        image->setPixmap(QPixmap(":/icons/light/32x32/line.png"));

        QHBoxLayout *box = new QHBoxLayout();
        box->addWidget(label);
        box->addWidget(image);

        label->setFixedHeight(40);

        ui->verticalLayout_2->addLayout(box);
    }

    listView->setModel(model);

    m_filterList->append(listView);
    ui->verticalLayout_2->addWidget(listView);

    listView->setFont(QFont(font().family(), 8));   // 改变字体大小
    listView->setFocusPolicy(Qt::ClickFocus);
    listView->setViewMode(QListView::IconMode);
    listView->setGridSize(QSize(100, 85));         // 120,100    ,300/3-5    95,90
    listView->setUniformItemSizes(true);
    listView->setResizeMode(QListView::Adjust);
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    listView->setContentsMargins(0, 5, 0, 5);
    listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listView->setStyleSheet(
                "QListView::item:selected{background-color:rgb(192,72,44); color:rgb(255,255,255);border-radius:4px;}"
                "QListView::item:hover{background-color:rgb(192,72,44); color:rgb(255,255,255);border-radius:4px;}"
                "QListView{background-color:transparent;color:rgb(214,214,214);}");

    connect(listView, SIGNAL(pressed(const QModelIndex&)), this, SLOT(onListviewPressed(const QModelIndex&)));
    connect(listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onListviewCustomContextMenuRequested(const QPoint&)));

    connect(listView, SIGNAL(mouseMove()), this, SLOT(positionAddButton()));
    connect(listView, SIGNAL(mouseLeave()), this, SLOT(hideAddButton()));
    connect(listView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onListviewDoubleClicked(const QModelIndex&)));
}

void VideoFilterDock::onListviewPressed(const QModelIndex &index)
{
    if(!sender())
        return;
    VideoFilterListView *listView = qobject_cast<VideoFilterListView*>(sender());
    if(m_currentListView && m_currentListView!=listView)
    {
        m_currentListView->clearSelection();
    }

    m_currentListView = listView;
    m_currentIndex = index;

    VideoFilterListVideoModel *model = qobject_cast<VideoFilterListVideoModel *>(listView->model());
    Q_ASSERT(model);
    if(model) {
        FilterItemInfo *filterItemInfo = model->fileAt(m_currentIndex.row());
        int filterIndex = filterItemInfo->filterIndex();
        previewFilter(filterIndex);
    }
}

void VideoFilterDock::onListviewClicked(const QModelIndex &)
{
////    Q_ASSERT(m_effectFile);   // 不能加，默认下可以为空，也就是不播放文件
//    Q_ASSERT(m_mainWindow);
//    if(m_effectFile && m_mainWindow)
//    {
//        m_mainWindow->playFile(m_effectFile);
//    }
}

void VideoFilterDock::onListviewDoubleClicked(const QModelIndex &index) {
    if(!sender())
        return;
    VideoFilterListView *listView = qobject_cast<VideoFilterListView*>(sender());
    if(m_currentListView && m_currentListView!=listView)
    {
        m_currentListView->clearSelection();
    }

    m_currentListView = listView;
    m_currentIndex = index;

    VideoFilterListVideoModel *model = qobject_cast<VideoFilterListVideoModel *>(listView->model());
    Q_ASSERT(model);
    if(model) {
        FilterItemInfo *filterItemInfo = model->fileAt(m_currentIndex.row());
        int filterIndex = filterItemInfo->filterIndex();
        addFilter(filterIndex);
    }
}

void VideoFilterDock::onListviewCustomContextMenuRequested(const QPoint &pos)
{
    Q_ASSERT(m_currentIndex.isValid());
    Q_ASSERT(m_currentListView);
    if(m_currentIndex.isValid() && m_currentListView && m_currentListView->indexAt(pos)==m_currentIndex)
    {
        QMenu menu(this);
        menu.addAction(ui->actionAddToTimeline);
        menu.exec(m_currentListView->mapToGlobal(pos));
    }
}

void VideoFilterDock::on_actionAddToTimeline_triggered()
{
//    Q_ASSERT(m_effectFile);
//    Q_ASSERT(m_mainWindow);
//    if(m_effectFile && m_mainWindow)
//    {
//        m_mainWindow->addToTimeLine(m_effectFile);
//    }
}

void VideoFilterDock::on_comboBox_2_activated(int index)
{
    QLayoutItem *item = ui->verticalLayout_2->itemAt(index*2);
    if (item == nullptr) {
        return;
    }

    QLayout *layout = item->layout();
    if (layout == nullptr) {
        return;
    }

    QLayoutItem *layoutItem = layout->itemAt(0);
    if (layoutItem == nullptr) {
        return;
    }

    QWidget *widget = layoutItem->widget();
    if (widget == nullptr) {
        return;
    }

    ui->scrollArea->verticalScrollBar()->setValue(widget->y());
}

void VideoFilterDock::on_VideoFilterDock_visibilityChanged(bool visible)
{
    if (visible) {
//        onListviewClicked(QModelIndex());
//        onListviewPressed(QModelIndex());
//        if(m_effectFile && m_mainWindow)
//        {
//            m_mainWindow->playFile(m_effectFile);
//        }

        resizeEvent(nullptr);   // 切换dock后listView大小会随dock变化
    }
}

void VideoFilterDock::updateVideoFilters(VideoFilter_Info *filterInfos, int nFilterCount)
{
    Q_ASSERT(filterInfos);
    if(!filterInfos) return;

    if(!m_mainWindow)
    {
        return;
    }

    //分类filter
    for (int i = 0; i < nFilterCount; i++) {
        if ((filterInfos[i].visible == true) && (QString(filterInfos[i].type) != "")) {//视频滤镜
            if (!m_filtersInfoMap.keys().contains(QString(filterInfos[i].type))) {
                VideoFilterListVideoModel *model = new VideoFilterListVideoModel(m_mainWindow, this);
                m_filtersInfoMap.insert(QString(filterInfos[i].type), model);
            }

            FilterItemInfo *filterInfo = new FilterItemInfo();
            filterInfo->setVisible(filterInfos[i].visible);
            filterInfo->setName(QString(filterInfos[i].name));
            filterInfo->setFilterType(QString(filterInfos[i].type));
            QString tempPath = QString(filterInfos[i].imageSourcePath);
            QString path = tempPath.right(tempPath.length() - 3);
            filterInfo->setImageSourcePath(path);
            filterInfo->setFilterIndex(i);

            VideoFilterListVideoModel *tempModel = m_filtersInfoMap.value(QString(filterInfos[i].type));
            tempModel->append(filterInfo);
        }
    }

    //添加model到listview
    QMap<QString, VideoFilterListVideoModel *>::const_iterator i;
    for (i = m_filtersInfoMap.constBegin(); i != m_filtersInfoMap.constEnd(); ++i) {
        ui->comboBox_2->addItem(i.key());
        appendListViewAndLabel(i.value(), i.key());
    }
    return;
}

void VideoFilterDock::updateAudioFilters(VideoFilter_Info *filterInfos, int nFilterCount)
{
    Q_ASSERT(filterInfos);
    if(!filterInfos) return;

    if(!m_mainWindow)
    {
        return;
    }

    //分类filter
    for (int i = 0; i < nFilterCount; i++) {
        if ((filterInfos[i].visible == true) && (QString(filterInfos[i].type) == "")) {//音频滤镜
            if (!m_filtersInfoMap.keys().contains(QString(filterInfos[i].type))) {
                VideoFilterListVideoModel *model = new VideoFilterListVideoModel(m_mainWindow, this);
                m_filtersInfoMap.insert(QString(filterInfos[i].type), model);
            }

            FilterItemInfo *filterInfo = new FilterItemInfo();
            filterInfo->setVisible(filterInfos[i].visible);
            filterInfo->setName(QString(filterInfos[i].name));
            filterInfo->setFilterType(QString(filterInfos[i].type));
            QString tempPath = QString(filterInfos[i].imageSourcePath);
            QString path = tempPath.right(tempPath.length() - 3);
            filterInfo->setImageSourcePath(path);
            filterInfo->setFilterIndex(i);

            VideoFilterListVideoModel *tempModel = m_filtersInfoMap.value(QString(filterInfos[i].type));
            tempModel->append(filterInfo);
        }
    }

    //添加model到listview
    QMap<QString, VideoFilterListVideoModel *>::const_iterator i;
    for (i = m_filtersInfoMap.constBegin(); i != m_filtersInfoMap.constEnd(); ++i) {
        ui->comboBox_2->addItem(i.key());
        appendListViewAndLabel(i.value(), i.key());
    }

    QSpacerItem *spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_2->addItem(spacerItem);
    return;
}

void VideoFilterDock::addFilter(int filterIndex) {
    m_mainWindow->addFilter(filterIndex);
}

void VideoFilterDock::previewFilter(int filterIndex) {
    m_mainWindow->previewFilter(filterIndex);
}

//static VideoFilterDock *instance = nullptr;
//初始化模块
//参数，main 主程序接口对象
//返回界面对象
QDockWidget *VideoFilterDock_initModule(MainInterface *main, int filterDockType)
{
//    if (instance == nullptr)
//        instance = new VideoFilterDock(filterDockType, main);
//    return instance;
    return new VideoFilterDock(filterDockType, main);
}

//销毁模块
void VideoFilterDock_destroyModule()
{

}

int g_setVideoFiltersInfo(QDockWidget *videoDock, VideoFilter_Info *filterInfos, int nFilterCount)
{
    Q_ASSERT(filterInfos);
    if(!filterInfos) return 1;
    Q_ASSERT(videoDock);
    if(!videoDock) return 1;
    qobject_cast<VideoFilterDock *>(videoDock)->updateVideoFilters(filterInfos, nFilterCount);

    return 0;
}

int g_setAudioFiltersInfo(QDockWidget *audioDock, VideoFilter_Info *filterInfos, int nFilterCount)
{
    Q_ASSERT(filterInfos);
    if(!filterInfos) return 1;
    Q_ASSERT(audioDock);
    if(!audioDock) return 1;
    qobject_cast<VideoFilterDock *>(audioDock)->updateAudioFilters(filterInfos, nFilterCount);

    return 0;
}
