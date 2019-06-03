#include "desccontentedit.h"
#include <QLocale>
DescContentEdit::DescContentEdit(QWidget *parent) :
    QTextEdit(parent)
{

}

DescContentEdit::~DescContentEdit()
{

}
void DescContentEdit::inputMethodEvent(QInputMethodEvent *e)
{
    QTextEdit::inputMethodEvent(e);
    if(e->preeditString().isEmpty())
    {
        this->setPlaceholderText(tr("Detailed Information about how the problem occurred (optional)"));
        QString language = QLocale::system().name();
        if (language == "zh_CN")
        {
            this->setPlaceholderText("错误发生时的操作描述(可选)");
        }
    }
    else
    {
        this->setPlaceholderText(QString::null);
    }
}
