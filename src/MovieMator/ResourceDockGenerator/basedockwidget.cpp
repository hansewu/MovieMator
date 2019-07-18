#include "basedockwidget.h"
#include "ui_basedockwidget.h"
#include "baseitemdelegate.h"
#include "baselistview.h"
#include "baseitemmodel.h"

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

void BaseDockWidget::setupUi() {
    qDebug()<<"sll-----setupUi---start";
    //初始化及设置顶部控件，各子类可以定制
    setupOtherUi();

    //初始化及设置listview
    setupListView();
    qDebug()<<"sll-----setupUi---end";
}

void BaseDockWidget::setupOtherUi() {
    qDebug()<<"sll-----setupOtherUi---start";
    qDebug()<<"sll-----setupOtherUi---end";
}

void BaseDockWidget::setupListView() {
    qDebug()<<"sll-----setupListView---start";

    m_allClassesListView = new QMap<QString, BaseListView *>;

    QMap<QString, BaseItemModel *> *allClassesItemModel = createAllClassesItemModel();

    if (hasClass()) {
        createAllClassesListView(allClassesItemModel, hasClass());
    } else {
        //无分类时移除分类控件
        ui->comboBox_class->setHidden(true);
        ui->classlabe->setHidden(true);

        BaseListView *listView = createListView(allClassesItemModel->first());
        m_allClassesListView->insert("Undefined", listView);
    }

    QSpacerItem *spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                              QSizePolicy::Expanding);
    ui->verticalLayout_scrollarea->addItem(spacerItem);
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

    ui->verticalLayout_scrollarea->addLayout(box);

    qDebug()<<"sll-----createClassesNameWidget---end";
}

void BaseDockWidget::createAllClassesListView(QMap<QString, BaseItemModel *> *allClassesItemModel,
        bool hasClass) {
    Q_UNUSED(hasClass);

    qDebug()<<"sll-----createAllClassesListView---start";
    //设置分类combobox，连接函数
    ui->comboBox_class->setStyleSheet("QComboBox { background-color:rgb(100,100,100);color:rgb(225,225,225); }");
    connect(ui->comboBox_class, SIGNAL(activated(int)), this, SLOT(onClassComboBoxActivated(int)));

    //创建各分类listview，并添加到UI
    QMap<QString, BaseItemModel *>::const_iterator iter;
    for (iter = allClassesItemModel->constBegin(); iter != allClassesItemModel->constEnd(); iter++) {
        //创建分类名控件
        createClassesNameWidget(iter.key());

        //设置分类combobox的数据项
        ui->comboBox_class->addItem(iter.key());

        //保存listview用于分类跳转
        BaseListView *listView = createListView(iter.value());
        m_allClassesListView->insert(iter.key(), listView);
    }
    qDebug()<<"sll-----createAllClassesListView---end";
}

BaseListView *BaseDockWidget::createListView(BaseItemModel *itemModel) {
    qDebug()<<"sll-----createListView---start";
    BaseItemDelegate *itemDelegate = new BaseItemDelegate(this);
    BaseListView *listView = new BaseListView(this);

    listView->setModel(itemModel);
    listView->setItemDelegate(itemDelegate);

    connect(itemDelegate, &BaseItemDelegate::addItem, this, &BaseDockWidget::addItemToTimeline);
    connect(itemDelegate, &BaseItemDelegate::selectItem, this, &BaseDockWidget::clickedItem);

    ui->verticalLayout_scrollarea->addWidget(listView);

    qDebug()<<"sll-----createListView---end";

    return listView;
}

void BaseDockWidget::addItemToTimeline(const QModelIndex &index) {
    qDebug()<<"sll-----addItemToTimeline---start";
    QAbstractItemModel *itemModel = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *standardItemModel = static_cast<BaseItemModel *>(itemModel);
    QStandardItem *standardItem = standardItemModel->itemFromIndex(index);

    addToTimeline(standardItem);
    qDebug()<<"sll-----addItemToTimeline---end";
}

void BaseDockWidget::clickedItem(const QModelIndex &index) {
    qDebug()<<"sll-----clickedItem---start";
    QAbstractItemModel *itemModel = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *standardItemModel = static_cast<BaseItemModel *>(itemModel);
    QStandardItem *standardItem = standardItemModel->itemFromIndex(index);

    //因为每个分类由一个listview显示，因此在从一个分类中的选中项切换到另一个分类中的选中项时，需要手动清除前一个listview中的选中项
    QMap<QString, BaseListView *>::const_iterator iter;
    for (iter = m_allClassesListView->constBegin(); iter != m_allClassesListView->constEnd(); iter++) {
        BaseListView *listView = iter.value();
        BaseItemModel *tempItemModel = static_cast<BaseItemModel *>(listView->model());
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
    qDebug()<<"sll-----resizeEvent---start";
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

    //listview大小变化时，保证依然在当前分类的listview上
    onClassComboBoxActivated(ui->comboBox_class->currentIndex());

    QDockWidget::resizeEvent(event);

    qDebug()<<"sll-----resizeEvent---end";
}

//void BaseDockWidget::onClassComboBoxActivated(const QString &className) {
void BaseDockWidget::onClassComboBoxActivated(int index) {
    qDebug()<<"sll-----onClassComboBoxActivated---start";
    //每个分类对应两个控件。即：分了名和分类listview。因此通过combobox切换分类时，通过index * 2找到对应的分类控件位置，进行跳转
    QLayoutItem *item = ui->verticalLayout_scrollarea->itemAt(index * 2);
    Q_ASSERT(item);
    if (item) {
        Q_ASSERT(item->layout());
        Q_ASSERT(item->layout()->itemAt(0));
        QWidget *widget = item->layout()->itemAt(0)->widget();
        Q_ASSERT(widget);
        if(widget) {
            ui->scrollArea->verticalScrollBar()->setValue(widget->y());
        }
    }
    qDebug()<<"sll-----onClassComboBoxActivated---end";
}
