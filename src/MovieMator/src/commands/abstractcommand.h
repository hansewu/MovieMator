#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

#include <QUndoCommand>


class AbstractCommand : public QUndoCommand
{
public:
    AbstractCommand(QUndoCommand * parent = 0);
    void redo();
    void undo();

    virtual void redo_impl() = 0;
    virtual void undo_impl() = 0;
};

#endif // ABSTRACTCOMMAND_H
