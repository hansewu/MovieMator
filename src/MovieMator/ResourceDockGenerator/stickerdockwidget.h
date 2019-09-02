#ifndef STICKERDOCKWIDGET_H
#define STICKERDOCKWIDGET_H

#include "basedockwidget.h"
#include "maininterface.h"

#include <QJsonObject>

class MainInterface;
class QIcon;
class QComboBox;

class StickerDockWidget : public BaseDockWidget
{
    Q_OBJECT
public:
    // 构造函数
    explicit StickerDockWidget(MainInterface *pMainInterface = nullptr,
                               QWidget *pParent = nullptr);

    //用于StickerItemModel中创建拖拽数据
    static FILE_HANDLE createFileHandle(MainInterface *pMainInterface,
                                        const QString &strAnimationFilePath,
                                        const QString &strImageFilePath);

protected:
    void resizeEvent(QResizeEvent *pEvent);
    UnsortMap<QString, BaseItemModel *> *createAllClassesItemModel();
    void addItemToTimeline(const QStandardItem *pItem);
    void preview(const QStandardItem *pItem);
    void setupTopBarUi();

signals:
    void currentSelectedAnimationChanged(QString &strAnimationFilePath);

private slots:
    void onAnimationComboBoxCurrentIndexChanged(int nIndex);
    void onAnimationComboBoxActivated(int nIndex);

private:
    QIcon getListViewItemIcon(const QString strFilePath);
    void setupAnimationComboboxData();

    static QString getImageClassType(QString srcStr);

private:
    MainInterface   *m_pMainInterface;
    QComboBox       *m_pAnimationCombobox;
    QString         m_strCurrentSelectedImageFilePath;
};

#endif // STICKERDOCKWIDGET_H
