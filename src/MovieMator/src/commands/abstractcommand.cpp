#include "abstractcommand.h"

#include <Mlt.h>
#include <Logger.h>
#include <mltcontroller.h>

AbstractCommand::AbstractCommand(QUndoCommand * parent) : QUndoCommand (parent)
{

}

void AbstractCommand::redo()
{
    //clock_t begin, duration;
    //begin = clock();

    MLT.consumer()->set_cancelled(1);
    w_enter_critical();
    MLT.consumer()->set_cancelled(0);
    //duration = clock() - begin;
    //LOG_DEBUG() << "redo w_enter_critical ------ "<< duration;

    this->redo_impl();
    w_leave_critical();
}

void AbstractCommand::undo()
{
    MLT.consumer()->set_cancelled(1);
    w_enter_critical();
    MLT.consumer()->set_cancelled(0);
    this->undo_impl();
    w_leave_critical();
}
