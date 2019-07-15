#include "textdock.h"
#include "ui_textdock.h"
#include "settings.h"
#include "util.h"

#include <QMenu>
//#include <QFile>
#include <Logger.h>
#include <QScrollBar>
#include <QDomDocument>

//#include <QJsonArray>
//#include <QJsonDocument>
//#include <QJsonParseError>

TextDock::TextDock(MainInterface *main, QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::TextDock),
    m_mainWindow(main)
{
    LOG_DEBUG() << "begin";

    ui->setupUi(this);
    toggleViewAction()->setIcon(windowIcon());

    m_textList = new QList<TextListView *>;
    m_currentListView = nullptr;
    m_currentIndex = QModelIndex();
    m_currentFile = nullptr;

    QString templateDir = Util::resourcesPath() + "/template/";
    TranslationHelper::readJsonFile(templateDir + "textclass_name_translation_info.json", m_textClassNameTranslateInfo);

    // 文字模板列表
    QDir textDir(templateDir+"Text");
    QFileInfoList textFolderList = textDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for(int i=0; i<textFolderList.count(); i++)
    {
        QFileInfo textFolder = textFolderList.at(i);
        QFileInfoList textFileList = QDir(textFolder.absoluteFilePath()).entryInfoList(QDir::Files | QDir::NoSymLinks);

        if(textFileList.count()>0)
        {
            QString itemName = TranslationHelper::getTranslationStr(textFolder.fileName(), m_textClassNameTranslateInfo);
            ui->comboBox_2->addItem(itemName);
            createTextFileList(textFileList, itemName);
        }
    }

    // 实际上文件夹多了就用不上了
    QSpacerItem *spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_2->addItem(spacerItem);

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

TextDock::~TextDock()
{
    qDeleteAll(*m_textList);
    m_textList->clear();
    delete m_textList;
    m_textList = nullptr;

    m_mainWindow->destroyFileHandle(m_currentFile);

    delete ui;
}

void TextDock::resizeEvent(QResizeEvent *event)
{
    Q_ASSERT(m_textList);
    if(!m_textList)
    {
        return;
    }
    for(TextListView *listView : *m_textList)
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
void TextDock::positionAddButton()
{
    if(!sender())
        return;

    TextListView *listView = qobject_cast<TextListView*>(sender());
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

void TextDock::hideAddButton()
{
    m_addButton->setVisible(false);
}

// 鼠标进入按钮时设置 listView的 item为选中状态
// 通过设置鼠标进入按钮时设置 item为选中状态来维持 item的背景色
// 鼠标进入按钮后，item的悬浮效果会被清除
void TextDock::setSelection()
{
    if(!sender() || !sender()->parent())
    {
        return;
    }

    TextListView *listView = qobject_cast<TextListView *>(sender()->parent());
    listView->selectionModel()->select(listView->getModelIndex(), QItemSelectionModel::Select);
}

// 鼠标离开按钮时重置 listView的 item选中状态
void TextDock::resetSelection()
{
    if(!sender() || !sender()->parent())
    {
        return;
    }

    TextListView *listView = qobject_cast<TextListView *>(sender()->parent());
    listView->clearSelection();
    // 当前 lsitView的当前 item依然要处于选中状态
    if(listView==m_currentListView)
    {
        listView->selectionModel()->select(m_currentIndex, QItemSelectionModel::Select);
    }
//    QModelIndex index = listView->currentIndex();
//    if(index.isValid())
//    {
//        listView->selectionModel()->select(index, QItemSelectionModel::Select);
//    }
}

void TextDock::addToTimeline()
{
    Q_ASSERT(m_mainWindow);
    if(!sender() || !sender()->parent() || !m_mainWindow)
        return;

    TextListView *tmpListView = m_currentListView;       // 保存值
    QModelIndex tmpIndex = m_currentIndex;               // 保存值

    TextListView *listView = qobject_cast<TextListView*>(sender()->parent());
    // 同一个文件，如果选中的和添加的是同一个文件
    if(listView==m_currentListView && m_currentIndex==listView->getModelIndex())
    {
        if(m_currentFile)
        {
            m_mainWindow->addToTimeLine(m_currentFile);
            return;
        }
    }
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

    genCurrentTextFile();

    if(m_currentFile)
    {
        m_mainWindow->addToTimeLine(m_currentFile);
    }

    // 添加到时间线的动画没有被选中，把动画切换回选中的
    // 防止切换 dock时没有选中动画时播放了添加到时间线的动画
    m_currentListView = tmpListView;
    m_currentIndex = tmpIndex;
    genCurrentTextFile();
}

void TextDock::genCurrentTextFile()
{
    Q_ASSERT(m_mainWindow);
    if(!m_mainWindow)
    {
        return;
    }
    m_mainWindow->destroyFileHandle(m_currentFile);
    if(m_currentListView && m_currentIndex.isValid())
    {
        TextItemInfo *textFile = qobject_cast<TextListModel*>(m_currentListView->model())->fileAt(m_currentIndex.row());
        m_currentFile = m_mainWindow->openFile(textFile->textFilePath());

        QFile file(textFile->textFilePath());
        if(!file.exists())
        {
            return;
        }
        QDomDocument doc;
        if(!doc.setContent(&file))
        {
            file.close();
            return;
        }
        file.close();
        qDebug() << "xxxxxxxxx : " << doc.toString();
        m_currentFile = m_mainWindow->createFileWithXMLForDragAndDrop(doc.toString());
    }
}

void TextDock::createTextFileList(QFileInfoList &fileList, QString folderName)
{
    Q_ASSERT(m_mainWindow);
    if(!m_mainWindow)
    {
        return;
    }
    TextListModel *model = new TextListModel(m_mainWindow, this);
    for(QFileInfo file : fileList)
    {
        // 屏蔽掉非 xml文件
        if(file.fileName().endsWith("xml", Qt::CaseInsensitive)){
            TextItemInfo *itemInfo = new TextItemInfo();
            itemInfo->setTextFilePath(file.filePath());
            itemInfo->setThumbnailPath(":/icons/filters/Audio.jpg");
            model->append(itemInfo);
        }
    }

    appendListViewAndLabel(model, folderName);
}

void TextDock::appendListViewAndLabel(TextListModel *model, QString itemName)
{
    Q_ASSERT(model);
    Q_ASSERT(m_textList);
    if(!model || !m_textList)
    {
        return;
    }
    QLabel *label = new QLabel(itemName, this);
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label->setFixedHeight(40);
    QLabel *image = new QLabel(this);
    image->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    image->setScaledContents(true);     // 可以让图片随 label拉伸
    image->setPixmap(QPixmap(":/icons/light/32x32/line.png"));
    QHBoxLayout *box = new QHBoxLayout();
    box->addWidget(label);
    box->addWidget(image);

    TextListView *listView = new TextListView();
    listView->setModel(model);
    listView->setFont(QFont(font().family(), 8));   // 改变字体大小
    listView->setFocusPolicy(Qt::ClickFocus);
    listView->setViewMode(QListView::IconMode);
    listView->setGridSize(QSize(95, 90));         // 120,100    ,300/3-5
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

    m_textList->append(listView);
    ui->verticalLayout_2->addLayout(box);
    ui->verticalLayout_2->addWidget(listView);
}

void TextDock::onListviewPressed(const QModelIndex &index)
{
    Q_ASSERT(m_mainWindow);
    if(!sender() || !m_mainWindow)
        return;

    // 同一个 file就不操作，防止每次点击同一个文件都要重新生成 FILE_HANDLE
    if(m_currentIndex==index && m_currentFile)
    {
        m_mainWindow->playFile(m_currentFile);
        return;
    }

    TextListView *listView = qobject_cast<TextListView *>(sender());
    if(m_currentListView && m_currentListView!=listView)
    {
        m_currentListView->clearSelection();
//        m_currentListView->setCurrentIndex(QModelIndex());
    }

    m_currentListView = listView;
    m_currentIndex = index;

    genCurrentTextFile();

    if(m_currentFile)
    {
        m_mainWindow->playFile(m_currentFile);
    }
}

void TextDock::onListviewCustomContextMenuRequested(const QPoint &pos)
{
    if(!sender())
    {
        return;
    }
    TextListView *listView = qobject_cast<TextListView*>(sender());
    if(!listView->indexAt(pos).isValid())
    {
        return;
    }
    QMenu menu(this);
    menu.addAction(ui->actionAddToTimeline);
    menu.exec(listView->mapToGlobal(pos));
}

void TextDock::on_actionAddToTimeline_triggered()
{
    Q_ASSERT(m_mainWindow);
    Q_ASSERT(m_currentFile);
    if(m_mainWindow && m_currentFile)
    {
        m_mainWindow->addToTimeLine(m_currentFile);
    }
}

void TextDock::on_comboBox_2_activated(int index)
{
    QLayoutItem *item = ui->verticalLayout_2->itemAt(index*2);
    if(item)
    {
        QWidget *widget = item->layout()->itemAt(0)->widget();
        if(widget)
        {
            ui->scrollArea->verticalScrollBar()->setValue(widget->y());
        }
    }
}

void TextDock::on_TextDock_visibilityChanged(bool visible)
{
    Q_ASSERT(m_mainWindow);
    if(!m_mainWindow)
    {
        return;
    }
    if (visible) {
        resizeEvent(nullptr);   // 切换dock后listView大小会随dock变化
        if(m_currentFile)
        {
            m_mainWindow->playFile(m_currentFile);
        }
    }
}

static TextDock *instance = nullptr;
//初始化模块
//参数，main 主程序接口对象
//返回界面对象
QDockWidget *TextDock_initModule(MainInterface *main)
{
    if (instance == nullptr)
        instance = new TextDock(main);
    return instance;
}

//销毁模块
void TextDock_destroyModule()
{

}
