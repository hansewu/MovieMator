#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include "models/multitrackmodel.h"



class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = 0);
    static MainController& sharedInstance();

    MultitrackModel& multitrackModel() { return m_multitrackModel;}

signals:

public slots:
    int initPythonQt();
    void evalFile(const QString& file);
    void appendToTimelineTrack(int trackIndex, const QStringList& files);

    //返回clipIndex，失败返回-1
    int appendToTimelineFromPath(int trackIndex, const QString &path);
    void insertTrack(int trackIndex);
    void removeTrack(int trackIndex);

    void splitClip(int trackIndex, int clipIndex, int position);

    Mlt::ClipInfo* getClipInfo(int trackIndex, int clipIndex);

    void addTransition(int trackIndex, int clipIndex, double duration);

    void setTransitionDuration(int trackIndex, int clipIndex, int duration);

    void setTransitionType(int trackIndex, int clipIndex, int transitionIndex);

    Mlt::Transition *getTransition(Mlt::Producer *producer, const QString &name);

    int getStartPositionOfClip(int trackIndex, int clipIndex);

    void setClipSpeed(int trackIndex, int clipIndex, double speed);

    void addFilter(int trackIndex, int clipIndex, const QString& filterID);
    void removeFilter(int trackIndex, int clipIndex, int row);

    void copyClip(int trackIndex, int clipIndex);
    void cutClip(int trackIndex, int clipIndex);
    void pasteClip(int trackIndex, int position);

    void setProfile(const QString & name);

    void undo();
    void redo();

    void exportFile(const QString& path);

    int videoTrackNums();
    int audioTrackNums();
    int trackNums();

    int clipsNum(int trackIndex);

private:
    MultitrackModel m_multitrackModel;

};


#define MAINCONTROLLER MainController::sharedInstance()
#endif // MAINCONTROLLER_H
