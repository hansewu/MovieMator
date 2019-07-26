#ifndef RECENTDOCKWIDGET_H
#define RECENTDOCKWIDGET_H

#include "basedockwidget.h"
#include "resourcedockgenerator_global.h"

#include <QAction>
#include <QSortFilterProxyModel>

class RecentDockWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    // 构造函数
    explicit RecentDockWidget(MainInterface *pMainInterface = nullptr,
                              QWidget *pParent = nullptr);
    ~RecentDockWidget();

    // 设置 model为 filterProxyModel，需要外部调用
    void setProxyModel();
    // 外部调用的添加文件并显示在 dock里
    void add(const QString &strFile);
    // 外部调用的移除功能
    QString remove(const QString &strFile);

protected:
    bool hasClass();

    void setupOtherUi();

    void showMenu(const QModelIndex &index);

    void preview(const QStandardItem *pItem);

    void addToTimeline(const QStandardItem *pItem);

    QMap<QString, BaseItemModel *> *createAllClassesItemModel();

private slots:
    // 右键移除槽函数
    void on_actionRemove_triggered();
    // 右键移除所有槽函数
    void on_actionRemoveAll_triggered();
    // 搜索框槽函数
    void on_lineEdit_textChanged(const QString &strSearch);

    void clickedItem(const QModelIndex &index);

    void addItemToTimeline(const QModelIndex &index);

    void onClassComboBoxActivated(int nIndex);

    void onDockWidgetVisibility(bool bVisible);

private:
    // 添加黑场视频
    void addBlackVideo();
    // 通过文件 strFile设置 item的内容，多个函数的共有内容
    int setItemModelInfo(const QString &strFile);
    // 将 filterProxyModel转换成原来的 model
    QModelIndex proxyToSource(const QModelIndex &index);
    // 获取 pItem的文件，多个函数的共有内容
    FILE_HANDLE getFileHandle(const QStandardItem *pItem);

    void hideModelTitle(int nIndex);

    void hideModelTitle(BaseItemModel *pModel);

    void showModelTitle(int nIndex);

private:
    MainInterface   *m_pMainInterface;
    // 右键菜单
    QAction         *m_pRemoveAction;       // 删除
    QAction         *m_pRemoveAllAction;    // 删除所有
    // 历史记录
    QStringList     m_recent;
    // 文件分类：样例、视频、音频、图片
    static const int nCount = 4;
    // 分类名称
    QString               m_strItemName[nCount];
    // 搜索过滤 Model
    QSortFilterProxyModel *m_pProxyModelArray[nCount];
    // 删除功能使用
    QStandardItem         *m_pCurrentItem;
};

#endif // RECENTDOCKWIDGET_H
