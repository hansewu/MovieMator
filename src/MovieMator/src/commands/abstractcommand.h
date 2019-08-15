#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

#include <QUndoCommand>
#include "models/multitrackmodel.h"


extern bool g_isInUndoRedoProcess; //是否正在undo和redo调用过程中

class AbstractCommand : public QUndoCommand
{
public:
    AbstractCommand(MultitrackModel &modelMultitrack, QUndoCommand * pParent = nullptr);
    void redo();
    void undo();
    virtual void redo_impl() = 0;
    virtual void undo_impl() = 0;

private:
    MultitrackModel     &m_modelMultitrack;
    QString             m_strXmlOriginal;
    QString             m_strXmlCurrent;
    TIMELINE_SELECTION  m_selectionOld;
    TIMELINE_SELECTION  m_selectionNew;
    bool                m_bisFirstRedo;
};

#endif // ABSTRACTCOMMAND_H
