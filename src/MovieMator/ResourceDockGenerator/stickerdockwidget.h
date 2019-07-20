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
    explicit StickerDockWidget(MainInterface *pMainInterface = nullptr, QWidget *pParent = nullptr);

    //用于StickerItemModel中创建拖拽数据
    static FILE_HANDLE createFileHandle(MainInterface *mainInterface,
                                        const QString &animationFilePath,
                                        const QString &imageFilePath);

protected:
    void resizeEvent(QResizeEvent *event);
    QMap<QString, BaseItemModel *> *createAllClassesItemModel();
    bool hasClass();
    void addToTimeline(const QStandardItem *item);
    void preview(const QStandardItem *item);
    void setupOtherUi();

signals:
    void currentSelectedAnimationChanged(QString &animationFilePath);

private slots:
    void onAnimationComboBoxCurrentIndexChanged(int index);
    void onAnimationComboBoxActivated(int index);

private:
    QIcon getListViewItemIcon(const QString filePath);
    void setupAnimationComboboxData();
//    FILE_HANDLE createFileHandle(const QString &animationFilePath, const QString &imageFilePath);
//    QString getImageClassType(QString srcStr, QJsonObject propertyInfo);
    static QString getImageClassType(QString srcStr);

private:
    MainInterface *m_mainInterface;
    QComboBox *m_animationCombobox;
    QJsonObject m_imageClassPropertyInfo;//根据不同的分类设置sizeAndPosition滤镜参数
    QString m_currentSelectedImageFilePath;
};

#endif // STICKERDOCKWIDGET_H
