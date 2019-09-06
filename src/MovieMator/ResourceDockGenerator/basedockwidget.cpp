#include "basedockwidget.h"
#include "ui_basedockwidget.h"
#include "baseitemdelegate.h"
#include "baselistview.h"
#include "baseitemmodel.h"

#include <qscrollbar.h>
#include <qlabel.h>
#include <qdebug.h>

BaseDockWidget::BaseDockWidget(QWidget *pParent) :
    QDockWidget(pParent),
    ui(new Ui::BaseDockWidget)
{
    qDebug()<<"sll-----BaseDockWidget构造---start";

    ui->setupUi(this);

    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrollArea->setStyleSheet("border:none;");
    ui->scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical{width:8px;background:rgba(0,0,0,0%);margin:0px,0px,0px,0px;}"
                            "QScrollBar::handle:vertical{width:8px;background:rgba(160,160,160,25%);border-radius:4px;min-height:20;}"
                            "QScrollBar::handle:vertical:hover{width:8px;background:rgba(160,160,160,50%);border-radius:4px;min-height:20;}");

    connect(this, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisibilityChanged(bool)));

    //设置分类combobox，连接函数
    ui->comboBox_class->setStyleSheet("QComboBox { background-color:rgb(100,100,100);color:rgb(225,225,225); }");
    connect(ui->comboBox_class, SIGNAL(activated(int)), this, SLOT(onClassComboBoxActivated(int)));

    m_pAllClassesListView = new QMap<QString, BaseListView *>;

    qDebug()<<"sll-----BaseDockWidget构造---end";
}

BaseDockWidget::~BaseDockWidget()
{
    qDebug()<<"sll-----BaseDockWidget析构---start";

    qDeleteAll(*m_pAllClassesListView);
    m_pAllClassesListView->clear();
    m_pAllClassesListView = nullptr;

    delete ui;

    qDebug()<<"sll-----BaseDockWidget析构---end";
}

void BaseDockWidget::setupUi()
{
    qDebug()<<"sll-----setupUi---start";

    //初始化及设置顶部控件，各子类可以定制
    setupTopBarUi();

    //创建所有分类的数据
    UnsortMap<QString, BaseItemModel *> *pAllClassesItemModel = createAllClassesItemModel();

    //创建所有分类的UI
    setupAllClassesUi(pAllClassesItemModel);

    qDebug()<<"sll-----setupUi---end";
}

void BaseDockWidget::setupTopBarUi()
{
    qDebug()<<"sll-----setupOtherUi---start";
    qDebug()<<"sll-----setupOtherUi---end";
}

void BaseDockWidget::showMeun(const QStandardItem *pItem, const QPoint &position)
{
    Q_UNUSED(pItem);
    Q_UNUSED(position);
}

void BaseDockWidget::clearSelectionOfNotCurrentListView(const QModelIndex &index)
{
    QMap<QString, BaseListView *>::const_iterator iter;
    for (iter = m_pAllClassesListView->constBegin(); iter != m_pAllClassesListView->constEnd(); iter++)
    {
        BaseListView *pListView = iter.value();
        if(pListView)
        {
            if (index.model() != pListView->model())
            {
                pListView->clearSelection();
            }
        }
    }
}

void BaseDockWidget::setupAllClassesUi(UnsortMap<QString, BaseItemModel *> *pAllClassesItemModel)
{
    //创建所有分类的listview
    Q_ASSERT(pAllClassesItemModel);
    if (pAllClassesItemModel == nullptr)
    {
        return;
    }

    bool hasClass = (pAllClassesItemModel->count() > 1) ? true : false;

    if (hasClass)
    {
        //创建各分类listview，并添加到UI
        for (QString strItemName : pAllClassesItemModel->keys())
        {
            //创建分类名控件
            QHBoxLayout *classLabelLayout = createClassLabel(strItemName);
            ui->verticalLayout_scrollarea->addLayout(classLabelLayout);

            //设置分类combobox的数据项
            ui->comboBox_class->addItem(strItemName);

            //创建一个分类的lsitview
            BaseListView *pOneClassListView = createClassListView(pAllClassesItemModel->value(strItemName));
            ui->verticalLayout_scrollarea->addWidget(pOneClassListView);

            //保存listview到map
            m_pAllClassesListView->insert(strItemName, pOneClassListView);
        }
    }
    else
    {
        //无分类时移除分类控件
        ui->comboBox_class->setHidden(true);

        //创建一个分类的lsitview
        QString strItemName             = pAllClassesItemModel->keys().first();
        BaseListView *pOneClassListView = createClassListView(pAllClassesItemModel->value(strItemName));
        ui->verticalLayout_scrollarea->addWidget(pOneClassListView);

        //保存listview到map，
        m_pAllClassesListView->insert(strItemName, pOneClassListView);
    }

    //在dock底部添加伸缩布局控件，控制dock中控件的数值布局是靠顶部的
    QSpacerItem *pSpacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum,
                                              QSizePolicy::Expanding);
    ui->verticalLayout_scrollarea->addItem(pSpacerItem);
}

QHBoxLayout *BaseDockWidget::createClassLabel(const QString &strClassName)
{
    QLabel *pClassLabel = new QLabel(strClassName, this);
    pClassLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pClassLabel->setFixedHeight(40);

    QLabel *pImageLabel = new QLabel(this);
    pImageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    pImageLabel->setScaledContents(true);     // 可以让图片随 label拉伸
    pImageLabel->setPixmap(QPixmap(":/icons/light/32x32/line.png"));

    QHBoxLayout *pHBoxLayout = new QHBoxLayout();
    pHBoxLayout->addWidget(pClassLabel);
    pHBoxLayout->addWidget(pImageLabel);

    return pHBoxLayout;
}

BaseListView *BaseDockWidget::createClassListView(BaseItemModel *pItemModel)
{
    Q_ASSERT(pItemModel);
    if (pItemModel == nullptr)
    {
        return nullptr;
    }

    BaseItemDelegate *pItemDelegate  = new BaseItemDelegate(this);
    BaseListView *pListView          = new BaseListView(this);

    pListView->setModel(pItemModel);
    pListView->setItemDelegate(pItemDelegate);

    connect(pItemDelegate, &BaseItemDelegate::addItem, this, &BaseDockWidget::onLeftClickedAddButtonInItem);
    connect(pItemDelegate, &BaseItemDelegate::selectItem, this, &BaseDockWidget::onLeftClickedItem);
    connect(pItemDelegate, &BaseItemDelegate::rightClickItem, this, &BaseDockWidget::onRightClickedItem);
    connect(pItemDelegate, &BaseItemDelegate::doubleClickItem, this, &BaseDockWidget::onDoubleClickedItem);

    return pListView;
}

void BaseDockWidget::onLeftClickedAddButtonInItem(const QModelIndex &index)
{
    qDebug()<<"sll-----addItemToTimeline---start";

    QAbstractItemModel *pItemModel       = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *pStandardItemModel    = static_cast<BaseItemModel *>(pItemModel);
    QStandardItem *pStandardItem         = pStandardItemModel->itemFromIndex(index);

    addItemToTimeline(pStandardItem);

    // 清空非当前选中状态
    clearSelectionOfNotCurrentListView(index);

    qDebug()<<"sll-----addItemToTimeline---end";
}

void BaseDockWidget::onLeftClickedItem(const QModelIndex &index)
{
    qDebug()<<"sll-----clickedItem---start";

    QAbstractItemModel *itemModel       = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *standardItemModel    = static_cast<BaseItemModel *>(itemModel);
    QStandardItem *standardItem         = standardItemModel->itemFromIndex(index);

    //因为每个分类由一个listview显示，因此在从一个分类中的选中项切换到另一个分类中的选中项时，需要手动清除前一个listview中的选中项
    clearSelectionOfNotCurrentListView(index);

    preview(standardItem);

    qDebug()<<"sll-----clickedItem---end";
}

void BaseDockWidget::onRightClickedItem(const QModelIndex &index, const QPoint &position)
{
    qDebug()<<"sll-----showMenu---start";

    QAbstractItemModel *pItemModel       = const_cast<QAbstractItemModel *>(index.model());
    BaseItemModel *pStandardItemModel    = static_cast<BaseItemModel *>(pItemModel);
    QStandardItem *pStandardItem         = pStandardItemModel->itemFromIndex(index);

    showMeun(pStandardItem, position);

    qDebug()<<"sll-----showMenu---end";
}

void BaseDockWidget::onDoubleClickedItem(const QModelIndex &index)
{
    // 调用添加按钮的功能
    onLeftClickedAddButtonInItem(index);
}

//FIXME:此函数需要优化，存在魔法数字
void BaseDockWidget::resizeEvent(QResizeEvent *pEvent)
{
    qDebug()<<"sll-----resizeEvent---start";

    QMap<QString, BaseListView *>::const_iterator iter;
    for (iter = m_pAllClassesListView->constBegin(); iter != m_pAllClassesListView->constEnd(); iter++)
    {
        BaseListView *pListView = iter.value();
        pListView->setFixedWidth(ui->scrollArea->width() - 5);

        int nGridHeight = pListView->gridSize().height();
        int nGridWidth = pListView->gridSize().width();
        if (nGridWidth <= 0)
        {
            continue;
        }

        int nListViewWidth = pListView->size().width();
        int nColumns = (nListViewWidth / nGridWidth) < 1 ? 1 : (nListViewWidth / nGridWidth);
        int nRowCount = pListView->model()->rowCount();
        int nRows = (nRowCount % nColumns) > 0 ? (nRowCount / nColumns + 1) : (nRowCount / nColumns);

        pListView->setFixedHeight(nRows * nGridHeight);
    }

    //listview大小变化时，保证依然在当前分类的listview上
    onClassComboBoxActivated(ui->comboBox_class->currentIndex());

    QDockWidget::resizeEvent(pEvent);

    qDebug()<<"sll-----resizeEvent---end";
}

void BaseDockWidget::onClassComboBoxActivated(int nIndex) {
    qDebug()<<"sll-----onClassComboBoxActivated---start";

    if (nIndex < 0)
    {
        return;
    }

    //每个分类对应两个控件。即：分了名和分类listview。因此通过combobox切换分类时，通过index * 2找到对应的分类控件位置，进行跳转
    QLayoutItem *pLayoutItem = ui->verticalLayout_scrollarea->itemAt(nIndex * 2);
    Q_ASSERT(pLayoutItem);
    if (pLayoutItem)
    {
        Q_ASSERT(pLayoutItem->layout());
        if (pLayoutItem->layout() == nullptr)
        {
            return;
        }

        Q_ASSERT(pLayoutItem->layout()->itemAt(0));
        if (pLayoutItem->layout()->itemAt(0) == nullptr)
        {
            return;
        }

        QWidget *pWidget = pLayoutItem->layout()->itemAt(0)->widget();
        Q_ASSERT(pWidget);
        if (pWidget == nullptr)
        {
            return;
        }

        ui->scrollArea->verticalScrollBar()->setValue(pWidget->y());
    }

    qDebug()<<"sll-----onClassComboBoxActivated---end";
}

void BaseDockWidget::onDockWidgetVisibilityChanged(bool bVisible)
{
    if(bVisible)
    {
        resizeEvent(nullptr);
    }
}
