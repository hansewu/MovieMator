/****************************************************************************
**
** Copyright (c) 2007 Trolltech ASA <info@trolltech.com>
**
** Use, modification and distribution is allowed without limitation,
** warranty, liability or support of any kind.
**
****************************************************************************/

#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class QToolButton;

class LineEditClear : public QLineEdit
{
    Q_OBJECT

public:
    LineEditClear(QWidget *parent = nullptr);
    void setStyleSheets(const QString &defaultStyleSheet, const QString &editingStyleSheet);

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void updateCloseButton(const QString &text);

private:
    QToolButton *clearButton;
    QString m_defaultStyleSheet;
    QString m_editingStyleSheet;
};

#endif // LIENEDIT_H

