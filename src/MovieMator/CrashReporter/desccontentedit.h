#ifndef DESCCONTENTEDIT_H
#define DESCCONTENTEDIT_H

#include <QTextEdit>
#include <QInputMethodEvent>

class DescContentEdit : public QTextEdit
{
public:
    DescContentEdit(QWidget *parent = 0);
    ~DescContentEdit();

protected:
    virtual void inputMethodEvent(QInputMethodEvent *e);
};

#endif // DESCCONTENTEDIT_H
