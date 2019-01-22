
#include "templateeditordock.h"
#include "settings.h"
#include "util.h"

#include <Logger.h>
#include <QFileDialog>

#include <qmlutilities.h>
#include <QQmlContext>
#include <qmlview.h>
#include <qmlmetadata.h>
#include <QQmlEngine>
#include <QUrl>

#include "thumbnailprovider.h"

TemplateEditorDock::TemplateEditorDock(MainInterface *main, QWidget *parent)
    : QDockWidget(parent)
    , m_mainWindow(main)
    , m_qview(QmlUtilities::sharedEngine(), this)
{
    LOG_DEBUG() << "begin";

    m_fileList = new QList<FILE_HANDLE>;

    setObjectName("TemplateEditorDock");
    m_qview.setFocusPolicy(Qt::StrongFocus);
    setWidget(&m_qview);

    QmlUtilities::setCommonProperties(m_qview.rootContext());
    m_qview.rootContext()->setContextProperty("view", new QmlView(m_qview.quickWindow()));
    m_qview.rootContext()->setContextProperty("templateEditorDock", this);
    m_qview.engine()->addImageProvider(QString("editorThumbnail"), new ThumbnailProvider(main));

    resetQview();
}

TemplateEditorDock::~TemplateEditorDock()
{
    qDeleteAll(*m_fileList);
    m_fileList->clear();
    delete m_fileList;
    m_fileList = nullptr;
}

void TemplateEditorDock::setFileList(QList<FILE_HANDLE> fileList)
{
    if(!m_fileList)
    {
        m_fileList = new QList<FILE_HANDLE>();
    }

    m_fileList->clear();
    for(int i=0; i<fileList.count(); i++)
    {
        m_fileList->append(fileList[i]);
    }
    emit refreshView();
}

int TemplateEditorDock::count() const
{
    return m_fileList ? m_fileList->count() : 0;
}

QString TemplateEditorDock::fileName(int index) const
{
    Q_ASSERT(index>=0 && index<m_fileList->count());
    return Util::baseName(m_mainWindow->getFileName(m_fileList->at(index)));
}

QString TemplateEditorDock::filePath(int index) const
{
    Q_ASSERT(index>=0 && index<m_fileList->count());
    return m_mainWindow->getFileName(m_fileList->at(index));
}

void TemplateEditorDock::remove(int index)
{
    Q_ASSERT(index>=0 && index<m_fileList->count());
    FILE_HANDLE fileHandle = m_mainWindow->resetFileInTemplate(index);
    if(fileHandle)
    {
        m_fileList->removeAt(index);
        m_fileList->insert(index, fileHandle);
        emit refreshThumbnail(index);
    }
}

void TemplateEditorDock::chooseFile(int index)
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Choose file"));
    fileDialog->setDirectory(".");
    if(fileDialog->exec())
    {
        FILE_HANDLE fileHandle = m_mainWindow->openFile(fileDialog->selectedFiles().at(0));
        replaceFile(index, fileHandle);
    }
}

void TemplateEditorDock::replaceFile(int flag, int index, QString str)
{
    FILE_HANDLE destFile = nullptr;
    if(flag==0)
    {   // 从文件夹拖放
        QString filePath = QUrl(str).path();
        if(filePath[0]=='/' && filePath[2]==':')
        {
            filePath = filePath.right(filePath.size()-1);
        }
        destFile = m_mainWindow->openFile(filePath);
    }
    else if(flag==1)
    {   // 从历史记录拖放
        destFile = m_mainWindow->createFileWithXMLForDragAndDrop(str);
    }
    replaceFile(index, destFile);
}

void TemplateEditorDock::resetQview()
{
    QDir viewPath = QmlUtilities::qmlDir();
    viewPath.cd("views");
    viewPath.cd("filter");
    m_qview.engine()->addImportPath(viewPath.path());

    QDir modulePath = QmlUtilities::qmlDir();
    modulePath.cd("modules");
    m_qview.engine()->addImportPath(modulePath.path());

    m_qview.setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_qview.quickWindow()->setColor(palette().window().color());
    QUrl source = QUrl::fromLocalFile(viewPath.absoluteFilePath("replacefile.qml"));
    m_qview.setSource(source);
}

void TemplateEditorDock::replaceFile(int index, FILE_HANDLE destFile)
{
    Q_ASSERT(index>=0 && index<m_fileList->count() /*&& destFile*/);
    if(destFile)
    {
        m_fileList->removeAt(index);
        m_fileList->insert(index, destFile);
        m_mainWindow->replaceFileInTemplate(index, destFile);
        emit refreshThumbnail(index);
    }
}

static TemplateEditorDock *instance = nullptr;
//初始化模块
//参数，main 主程序接口对象
//返回界面对象
QDockWidget *TemplateEditorDock_initModule(MainInterface *main)
{
    if (instance == nullptr)
        instance = new TemplateEditorDock(main);
    return instance;
}
// 销毁模块
void TemplateEditorDock_destroyModule()
{

}
// 初始化界面
void TemplateEditorDock_initTemplateEditor(QList<FILE_HANDLE> fileList)
{
    instance->setFileList(fileList);
}

