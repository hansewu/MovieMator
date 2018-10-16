#ifndef CUSTOMTABWIDGET
#define CUSTOMTABWIDGET

#endif // CUSTOMTABWIDGET

#include <QTabBar>
#include <QTabWidget>
#include <QStylePainter>
#include <QStyleOptionTabV3>

class CustomTabBar : public QTabBar
{
public:
    explicit CustomTabBar(QWidget* parent=0) : QTabBar(parent)
    {
        setIconSize(QSize(80, 40));
    }

protected:
    QSize tabSizeHint(int) const
    {
        return QSize(80, 30);
    }
    void paintEvent(QPaintEvent *)
    {
        QStylePainter p(this);
        for (int index = 0; index < count(); index++)
        {
            QStyleOptionTabV3 tab;
            initStyleOption(&tab, index);

            QIcon tempIcon = tab.icon;
            QString tempText = tab.text;
            tab.icon = QIcon();
            tab.text = QString();

            p.drawControl(QStyle::CE_TabBarTab, tab);

            QPainter painter;
            painter.begin(this);
            QRect tabrect = tabRect(index);
            tabrect.adjust(0, 8, 0, -8);
            painter.drawText(tabrect, Qt::AlignBottom | Qt::AlignHCenter, tempText);
            tempIcon.paint(&painter, 0, tabrect.top(), tab.iconSize.width(), tab.iconSize.height(), Qt::AlignTop | Qt::AlignHCenter);
            painter.end();
        }
    }
};

class CustomTabWidget : public QTabWidget
{
public:
    explicit CustomTabWidget(QWidget *parent = 0) : QTabWidget(parent)
    {
        setTabBar(new CustomTabBar());
    }
};
