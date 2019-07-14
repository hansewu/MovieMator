#include "basedockwidget.h"
#include "ui_basedockwidget.h"
#include "baseitemdelegate.h"
#include "baselistview.h"

#include <qstandarditemmodel.h>
#include <qscrollbar.h>
#include <qlabel.h>
#include <qdebug.h>

BaseDockWidget::BaseDockWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::BaseDockWidget) {
    qDebug()<<"sll-----BaseDockWidget构造---start";

    ui->setupUi(this);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setStyleSheet("border:none;");
    ui->scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{width:8px;background:rgba(0,0,0,0%);margin:0px,0px,0px,0px;}"
                            "QScrollBar::handle:vertical{width:8px;background:rgba(160,160,160,25%);border-radius:4px;min-height:20;}"
                            "QScrollBar::handle:vertical:hover{width:8px;background:rgba(160,160,160,50%);border-radius:4px;min-height:20;}");
    qDebug()<<"sll-----BaseDockWidget构造---end";
}

BaseDockWidget::~BaseDockWidget() {
    qDebug()<<"sll-----BaseDockWidget析构---start";
    qDeleteAll(*m_allClassesListView);
    m_allClassesListView->clear();
    m_allClassesListView = nullptr;

    delete ui;
    qDebug()<<"sll-----BaseDockWidget析构---end";
}

void BaseDockWidget::setupListView() {
    qDebug()<<"sll-----setupListView---start";

    m_allClassesListView = new QMap<QString, BaseListView *>;

    QMap<QString, QStandardItemModel *> *allClassesItemModel = createAllClassesItemModel();

    bool hanClass = hasClass();

    createAllClassesListView(allClassesItemModel, hanClass);

    qDebug()<<"sll-----setupListView---end";
}

void BaseDockWidget::createClassesNameWidget(const QString &className) {
    qDebug()<<"sll-----createClassesNameWidget---start";
    QLabel *classLabel = new QLabel(className, this);
    classLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    classLabel->setFixedHeight(40);

    QLabel *image = new QLabel(this);
    image->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    image->setScaledContents(true);     // 可以让图片随 label拉伸
    image->setPixmap(QPixmap(":/icons/light/32x32/line.png"));

    QHBoxLayout *box = new QHBoxLayout();
    box->addWidget(classLabel);
    box->addWidget(image);

    ui->verticalLayout_2->addLayout(box);

    qDebug()<<"sll-----createClassesNameWidget---end";
}

void BaseDockWidget::createAllClassesListView(QMap<QString, QStandardItemModel *> *allClassesItemModel,
        bool hasClass) {
    qDebug()<<"sll-----createAllClassesListView---start";
    QMap<QString, QStandardItemModel *>::const_iterator iter;
    for (iter = allClassesItemModel->constBegin(); iter != allClassesItemModel->constEnd(); iter++) {
        //创建分类控件
        createClassesNameWidget(iter.key());

        BaseListView *listView = createListView(iter.value());
        m_allClassesListView->insert(iter.key(), listView);

        if (hasClass) {//如果有分类才需要创建
            //创建分类名控件，并添加到布局中

            //创建分类combox分类控件，并添加到布局
        }

        //添加listview到布局中
    }

    QSpacerItem *spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    ui->verticalLayout_2->addItem(spacerItem);

    qDebug()<<"sll-----createAllClassesListView---end";
}

BaseListView *BaseDockWidget::createListView(QStandardItemModel *itemModel) {
    qDebug()<<"sll-----createListView---start";
    BaseItemDelegate *itemDelegate = new BaseItemDelegate(this);
    BaseListView *listView = new BaseListView(this);

    listView->setModel(itemModel);
    listView->setItemDelegate(itemDelegate);

    listView->setFont(QFont(font().family(), 8));   // 改变字体大小
    listView->setFocusPolicy(Qt::NoFocus);
    listView->setViewMode(QListView::IconMode);
    listView->setGridSize(QSize(126, 82));         // 120,100    ,300/3-5
    listView->setUniformItemSizes(true);
    listView->setResizeMode(QListView::Adjust);
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    listView->setContentsMargins(0, 5, 0, 5);
    listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listView->setStyleSheet(
                "QListView::item:selected{background-color:rgb(192,72,44); color:rgb(255,255,255);border-radius:4px;}"
                "QListView::item:hover{background-color:rgb(192,72,44); color:rgb(255,255,255);border-radius:4px;}"
                "QListView{background-color:transparent;color:rgb(214,214,214);}");

    connect(itemDelegate, &BaseItemDelegate::addItem, this, &BaseDockWidget::addItemToTimeline);
    connect(itemDelegate, &BaseItemDelegate::selectItem, this, &BaseDockWidget::clickedItem);

    ui->verticalLayout_2->addWidget(listView);

    qDebug()<<"sll-----createListView---end";

    return listView;
}

void BaseDockWidget::addItemToTimeline(const QModelIndex &index) {
    qDebug()<<"sll-----addItemToTimeline---start";
    QAbstractItemModel *itemModel = const_cast<QAbstractItemModel *>(index.model());
    QStandardItemModel *standardItemModel = static_cast<QStandardItemModel *>(itemModel);
    QStandardItem *standardItem = standardItemModel->itemFromIndex(index);

    addToTimeline(standardItem);
    qDebug()<<"sll-----addItemToTimeline---end";
}

void BaseDockWidget::clickedItem(const QModelIndex &index) {
    qDebug()<<"sll-----clickedItem---start";
    QAbstractItemModel *itemModel = const_cast<QAbstractItemModel *>(index.model());
    QStandardItemModel *standardItemModel = static_cast<QStandardItemModel *>(itemModel);
    QStandardItem *standardItem = standardItemModel->itemFromIndex(index);

    //清空其他listview上的选中状态
    QMap<QString, BaseListView *>::const_iterator iter;
    for (iter = m_allClassesListView->constBegin(); iter != m_allClassesListView->constEnd(); iter++) {
        BaseListView *listView = iter.value();
        QStandardItemModel *tempItemModel = static_cast<QStandardItemModel *>(listView->model());
        if (tempItemModel != standardItemModel) {
            listView->clearSelection();
        }
    }

    preview(standardItem);

    qDebug()<<"sll-----clickedItem---end";
}

void BaseDockWidget::addItemToDock(QString itemClass, QStandardItem *item) {
    qDebug()<<"sll-----addItemToDock---start";
    Q_UNUSED(itemClass);
    Q_UNUSED(item);
    qDebug()<<"sll-----addItemToDock---end";
}

void BaseDockWidget::resizeEvent(QResizeEvent *event) {
    QMap<QString, BaseListView *>::const_iterator iter;
    for (iter = m_allClassesListView->constBegin(); iter != m_allClassesListView->constEnd(); iter++) {
        BaseListView *listView = iter.value();
        listView->setFixedWidth(ui->scrollArea->width() - 5);
        int wSize = listView->gridSize().width();
        if(wSize <= 0) {
            continue;
        }
        int hSize = listView->gridSize().height();
        int width = listView->size().width();
        int columns = width/wSize<1 ? 1: width/wSize;
        int rowCount = listView->model()->rowCount();
        int rows = rowCount%columns>0 ? (rowCount/columns+1) : (rowCount/columns);
        listView->setFixedHeight(rows*hSize);
    }
    QDockWidget::resizeEvent(event);
}
