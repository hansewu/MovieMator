#ifndef ABSTRACTCOMMAND_H
#define ABSTRACTCOMMAND_H

#include <QUndoCommand>
#include "models/multitrackmodel.h"


extern bool g_isInUndoRedoProcess; //是否正在undo和redo调用过程中

class AbstractCommand : public QUndoCommand
{
public:
    AbstractCommand(MultitrackModel &model, QUndoCommand * parent = nullptr);
    void redo();
    void undo();
    void saveStateAsXmlFile(QString original,QString currrent,QString commandName);

    virtual void redo_impl() = 0;
    virtual void undo_impl() = 0;
    void refreshSelection();
private:
    MultitrackModel &m_model;
    QString         m_originalXml;
    QString         m_currentXml;
    Selection       m_oldSelection;
    Selection       m_newSelection;
    bool            m_bisFirstRedo;
};

#endif // ABSTRACTCOMMAND_H
