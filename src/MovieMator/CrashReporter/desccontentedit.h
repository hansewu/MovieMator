#ifndef DESCCONTENTEDIT_H
#define DESCCONTENTEDIT_H

#include <QTextEdit>
#include <QInputMethodEvent>

// 继承自 QTextEdit
// 用来处理占位符 placeholder在输入中文时不隐藏的问题
class DescContentEdit : public QTextEdit
{
public:
    DescContentEdit(QWidget *parent = nullptr);
    ~DescContentEdit();

protected:
    // 需要重写的函数，当 TextEdit有文本内容时隐藏占位符
    virtual void inputMethodEvent(QInputMethodEvent *e);
};

#endif // DESCCONTENTEDIT_H
