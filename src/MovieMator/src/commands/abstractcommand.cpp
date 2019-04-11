#include "abstractcommand.h"

#include <Mlt.h>

AbstractCommand::AbstractCommand(QUndoCommand * parent) : QUndoCommand (parent)
{

}

void AbstractCommand::redo()
{
    w_enter_critical();
    this->redo_impl();
    w_leave_critical();
}

void AbstractCommand::undo()
{
    w_enter_critical();
    this->undo_impl();
    w_leave_critical();
}
