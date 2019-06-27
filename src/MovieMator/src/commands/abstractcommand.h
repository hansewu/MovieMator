#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

#include <QUndoCommand>


class AbstractCommand : public QUndoCommand
{
public:
    AbstractCommand(QUndoCommand * parent = nullptr);
    void redo();
    void undo();

    virtual void redo_impl() = 0;
    virtual void undo_impl() = 0;
private:
    QString m_originalXml;
    QString m_currentXml;
};

#endif // ABSTRACTCOMMAND_H
