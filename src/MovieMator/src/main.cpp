/*
 * Copyright (c) 2011-2016 Meltytech, LLC
 * Author: Dan Dennedy <dan@dennedy.org>
 *
 * Copyright (c) 2016-2019 EffectMatrix Inc.
 * Author: vgawen <gdb_1986@163.com>
 * Author: Dragon-S <15919917852@163.com>
 * Author: wyl <wyl@pylwyl.local>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
//#include <PythonQt.h>
//#include <gui/PythonQtScriptingConsole.h>


#include <QtWidgets>
#include <QtGlobal>
#include "mainwindow.h"
#include <settings.h>
#include <Logger.h>
#include <FileAppender.h>
#include <ConsoleAppender.h>
#include <QSysInfo>
#include <QProcess>
#include <QCommandLineParser>
#include <framework/mlt_log.h>
#include "securitybookmark/transport_security_bookmark.h"
#include <qmlutilities.h>
#include <QQmlEngine>
#include <QProcess>
#include <registrationchecker.h>

#include "maincontroller.h"
#include "dialogs/mmsplashscreen.h"

#include <time.h>

#include "CrashHandler/CrashHandler.h"
#include "util.h"

#ifdef Q_OS_WIN
extern "C"
{
    // Inform the driver we could make use of the discrete gpu
    /*
     * 消除警告：
     * 静态变量需要声明为 static，非静态变量需要外部声明 extern
     * 这里需要导出给其他模块使用，因此只能使用 extern
     * 使用 extern时不建议声明时初始化，因此需要把声明和定义分开
     */
    __declspec(dllexport) extern DWORD NvOptimusEnablement;
    __declspec(dllexport) extern int AmdPowerXpressRequestHighPerformance;

    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif


static MMSplashScreen *g_splash = nullptr;

static void mlt_log_handler(void *service, int mlt_level, const char *format, va_list args)
{
    if (mlt_level > mlt_log_get_level() && mlt_level != 65536)
        return;

    enum Logger::LogLevel cuteLoggerLevel = Logger::Fatal;
    switch (mlt_level) {
    case MLT_LOG_DEBUG:
        cuteLoggerLevel = Logger::Trace;
        break;
    case MLT_LOG_ERROR:
    case MLT_LOG_FATAL:
    case MLT_LOG_PANIC:
        cuteLoggerLevel = Logger::Error;
        break;
    case MLT_LOG_INFO:
        cuteLoggerLevel = Logger::Info;
        break;
    case MLT_LOG_VERBOSE:
        cuteLoggerLevel = Logger::Debug;
        break;
    case MLT_LOG_WARNING:
        cuteLoggerLevel = Logger::Warning;
        break;
    case 65536:
        QString msg = QString().vsprintf(format, args);
        msg.replace('\n', "");
#if defined(Q_OS_WIN)
        msg.replace('/', "\\");
#endif
        g_splash->showMessage(msg, Qt::AlignHCenter | Qt::AlignBottom, Qt::white);
        qApp->processEvents();
        return;
    }
    QString message;
    mlt_properties properties = service? MLT_SERVICE_PROPERTIES(static_cast<mlt_service>(service)) : nullptr;
    if (properties) {
        char *mlt_type = mlt_properties_get(properties, "mlt_type");
        char *service_name = mlt_properties_get(properties, "mlt_service");
        char *resource = mlt_properties_get(properties, "resource");
        if (!resource || resource[0] != '<' || resource[strlen(resource) - 1] != '>')
            mlt_type = mlt_properties_get(properties, "mlt_type" );
        if (service_name)
            message = QString("[%1 %2] ").arg(mlt_type).arg(service_name);
        else
            message = QString().sprintf("[%s %p] ", mlt_type, service);
        if (resource)
            message.append(QString("\"%1\" ").arg(resource));
        message.append(QString().vsprintf(format, args));
        message.replace('\n', "");
    } else {
        message = QString().vsprintf(format, args);
        message.replace('\n', "");
    }
    Logger::write(cuteLoggerLevel, __FILE__, __LINE__, "MLT", message);
}

#pragma pack(push, 1)
/**
  * 消除字节对齐警告：warning: padding size of 'Application' with 7 bytes to alignment boundary
  * 由于系统目前的对齐模数是8个字节（#pragma pack(show) == 8），而Application中有一个bool型成员变量，因此修改补7个额外自己对齐，因此会报警告
  * 消除警告的方法是不使用默认的对齐方式，手动设置对齐方式为1个字节（#pragma pack(push, 1)）
  */
class Application : public QApplication
{
public:
    MainWindow* mainWindow;
    QTranslator qtTranslator;
    QTranslator qtBaseTranslator;
    QTranslator shotcutTranslator;
    QString resourceArg;
    bool isFullScreen;

    Application(int &argc, char **argv)
        : QApplication(argc, argv)
    {
        QDir dir(applicationDirPath());
        dir.cd("lib");
        dir.cd("qt5");
        addLibraryPath(dir.absolutePath());


        setOrganizationName("effectmatrix");
        setOrganizationDomain("effectmatrix.com");
        setApplicationName("MovieMator Pro");
//#if MOVIEMATOR_PRO
//        setApplicationName("MovieMator Pro");
//#else
//        setApplicationName("MovieMator");
//#endif
        setApplicationVersion(MOVIEMATOR_VERSION);
        setAttribute(Qt::AA_UseHighDpiPixmaps);
        setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);
#if defined(Q_OS_MAC)
        setAttribute(Qt::AA_DontShowIconsInMenus);
#endif


        // Startup logging.
        dir = QStandardPaths::standardLocations(QStandardPaths::DataLocation).first();
        if (!dir.exists()) dir.mkpath(dir.path());


        const QString logFileName = dir.filePath("moviemator-log.txt");
        QFile::remove(logFileName);
        FileAppender* fileAppender = new FileAppender(logFileName);
        fileAppender->setFormat("[%-7l] <%c> %m\n");
        Logger::registerAppender(fileAppender);
#ifndef NDEBUG
        // Only log to console in dev debug builds.
        ConsoleAppender* consoleAppender = new ConsoleAppender();
        consoleAppender->setFormat(fileAppender->format());
        Logger::registerAppender(consoleAppender);

        mlt_log_set_level(MLT_LOG_VERBOSE);
//        mlt_log_set_level(MLT_LOG_QUIET);
#else
        mlt_log_set_level(MLT_LOG_VERBOSE);
#endif
        mlt_log_set_callback(mlt_log_handler);

        // Log some basic info.
        LOG_INFO() << "Starting MovieMator version" << MOVIEMATOR_VERSION;
#if defined (Q_OS_WIN)
        LOG_INFO() << "Windows version" << QSysInfo::windowsVersion();
#elif defined(Q_OS_MAC)
        LOG_INFO() << "OS X version" << QSysInfo::macVersion();
#else
        LOG_INFO() << "Linux version";
#endif
        LOG_INFO() << "number of logical cores =" << QThread::idealThreadCount();
        LOG_INFO() << "locale =" << QLocale();
        LOG_INFO() << "install dir =" <<  applicationDirPath();

#if defined(Q_OS_WIN)
        if (Settings.playerGPU()) {
            QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
        } else if (Settings.drawMethod() >= Qt::AA_UseDesktopOpenGL &&
                   Settings.drawMethod() <= Qt::AA_UseSoftwareOpenGL) {
//            QCoreApplication::setAttribute(Qt::ApplicationAttribute(Settings.drawMethod()));
            QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
//             QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
        }
#endif
        // Load translations
        QString locale = Settings.language();
        dir = applicationDirPath();
    #if defined(Q_OS_MAC)
        dir.cdUp();
        dir.cd("Resources");
        dir.cd("translations");
    #elif defined(Q_OS_WIN)
        dir.cd("share");
        dir.cd("translations");
    #else
        dir.cdUp();
        dir.cd("share");
        dir.cd("moviemator");
        dir.cd("translations");
    #endif
        if (locale.startsWith("pt_"))
            locale = "pt";
        if (qtTranslator.load("qt_" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
            installTranslator(&qtTranslator);
        else if (qtTranslator.load("qt_" + locale, dir.absolutePath()))
            installTranslator(&qtTranslator);
        if (qtBaseTranslator.load("qtbase_" + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
            installTranslator(&qtBaseTranslator);
        else if (qtBaseTranslator.load("qtbase_" + locale, dir.absolutePath()))
            installTranslator(&qtBaseTranslator);
        if (shotcutTranslator.load("mm_" + Settings.language(), dir.absolutePath()))
            installTranslator(&shotcutTranslator);
        else
            qDebug()<<"failed to load translator file";

        qDebug()<<"mm_"<<Settings.language() << dir.absolutePath();

        QCommandLineParser parser;
        parser.addHelpOption();
        parser.addVersionOption();
#ifndef Q_OS_WIN
        QCommandLineOption fullscreenOption("fullscreen",
            QCoreApplication::translate("main", "Fill the screen with the MovieMator window."));
        parser.addOption(fullscreenOption);
#endif
        QCommandLineOption gpuOption("gpu",
            QCoreApplication::translate("main", "Use GPU processing."));
        parser.addOption(gpuOption);
        parser.addPositionalArgument("resource",
            QCoreApplication::translate("main", "A file to open."));


        QCommandLineOption pythonFileOption(QStringList() << "pythonfile" << "python-file",
                                            QCoreApplication::translate("main", "Execute Python file"),
                                            QCoreApplication::translate("main", "python"));
        parser.addOption(pythonFileOption);

        parser.process(arguments());
#ifdef Q_OS_WIN
        isFullScreen = false;
#else
        isFullScreen = parser.isSet(fullscreenOption);
#endif
        if (parser.isSet(gpuOption))
            Settings.setPlayerGPU(true);


        if (!parser.positionalArguments().isEmpty())
            resourceArg = parser.positionalArguments().first();

        QString pythonFile = parser.value(pythonFileOption);


    }

    ~Application()
    {
        delete mainWindow;
        LOG_DEBUG() << "exiting";
    }

protected:
    bool event(QEvent *event);
};
#pragma pack(pop)

bool Application::event(QEvent *event)
{
    if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *openEvent = static_cast<QFileOpenEvent*>(event);
        resourceArg = openEvent->file();
        return true;
    }
    else return QApplication::event(event);
}

bool removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists()) {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = QDir().rmdir(dirName);
    }
    return result;
}

bool copyDirectoryFiles(const QString &fromDir, const QString &toDir, bool coverFileIfExist, const QMap<QString, QString> filter) {
    QDir sourceDir(fromDir);
    QDir targetDir(toDir);
    if(!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
//        if(!targetDir.mkdir(targetDir.absolutePath())) {
//            return false;
//        }
        if(!targetDir.mkpath(targetDir.absolutePath())) {
            return false;
        }
    }

    QFileInfoList fileInfoList = sourceDir.entryInfoList();
    foreach(QFileInfo fileInfo, fileInfoList){
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..") {
             continue;
        }

        if(fileInfo.isDir()){    /**< 当为目录时，递归的进行copy */
            if(!copyDirectoryFiles(fileInfo.filePath(), targetDir.filePath(fileInfo.fileName()), coverFileIfExist, filter)) {
                return false;
            }
        } else {            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
            QString fileName_en = fileInfo.fileName();
            if (!fileName_en.isEmpty()) {
                if(coverFileIfExist && targetDir.exists(fileName_en)){
                    targetDir.remove(fileName_en);
                }

                QString fileName_zh = filter.value(fileName_en);
                if (!fileName_zh.isEmpty()) {
                    if(coverFileIfExist && targetDir.exists(fileName_zh)){
                        targetDir.remove(fileName_zh);
                    }
                }

                QString language = QLocale::system().name();
                QString targetFileName = fileName_en;
                if (language == "zh_CN" && !fileName_zh.isEmpty()) {
                    targetFileName = fileName_zh;
                }

                /// 进行文件copy
                if(!QFile::copy(fileInfo.filePath(), targetDir.filePath(targetFileName))){
                    return false;
                }
            }
        }
    }
    return true;
}

void copyTextFilterPresetFile() {
    const QMap<QString, QString> textFilterPresetNameMap = {
        {"(defaults)", "默认"},
        {"Bottom Left", "左下"},
        {"Bottom Right", "右下"},
        {"Lower Third", "底部字幕"},
        {"Slide In From Bottom", "从底划入"},
        {"Slide In From Left", "从左划入"},
        {"Slide In From Right", "从右划入"},
        {"Slide In From Top", "从顶划入"},
        {"Slide Out Bottom", "从底划出"},
        {"Slide Out Left", "从左划出"},
        {"Slide Out Right", "从右划出"},
        {"Slide Out Top", "从顶划出"},
        {"Slow Pan Down", "由大变小"},
        {"Slow Pan Left", "从左侧由大变小"},
        {"Slow Pan Right", "从右侧由大变小"},
        {"Slow Pan Up", "缓缓从上落下"},
        {"Slow Zoom In", "逐渐缩小"},
        {"Slow Zoom In, Pan Up Left", "沿着左侧逐渐放大"},
        {"Slow Zoom In. Pan Down Right", "沿着右下逐渐放大"},
        {"Slow Zoom Out, Pan Down Left", "向左上逐渐缩小"},
        {"Slow Zoom Out, Pan Up Right", "沿着右侧逐渐放大"},
        {"Slow Zoom Out", "逐渐放大"},
        {"Top Left", "左上"},
        {"Top Right", "右上"},
    };

    QDir applicationDir(qApp->applicationDirPath());
    QString filterPresetsPath = qApp->applicationDirPath() + "/share/mlt/presets/filter/dynamicText";
#ifdef Q_OS_MAC
    filterPresetsPath = qApp->applicationDirPath() + "/../Resources/share/mlt/presets/filter/dynamicText";
#endif
    QDir userDataDir(QStandardPaths::standardLocations(QStandardPaths::DataLocation).first());
    QString targetPath = userDataDir.path() + "/presets/dynamicText";
    copyDirectoryFiles(filterPresetsPath, targetPath, true, textFilterPresetNameMap);
}

int main(int argc, char **argv)
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
    QCoreApplication::setAttribute(Qt::AA_X11InitThreads);
    QCoreApplication::addLibraryPath("./lib");
#endif

    setenv("QT_DEVICE_PIXEL_RATIO", "auto", 1);
//    setenv("QT_SCALE_FACTOR", "2", 1);
    Application a(argc, argv);


#if defined (QT_NO_DEBUG) && defined (SHARE_VERSION) && !defined(Q_OS_MAC) //appstore版本不使用
    CrashManager::CrashHandler::instance()->Init(QDir::tempPath());
#endif

//   MMSplashScreen splash(QPixmap(":/splash.png"));
//    splash.showMessage(QCoreApplication::translate("main", "Loading plugins..."), Qt::AlignHCenter | Qt::AlignBottom, Qt::white);
//    splash.show();
// 宏定义警告，不消除
//#if MOVIEMATOR_FREE
//    g_splash = new MMSplashScreen(QPixmap(":/splash-free.png"));
//#else
    g_splash = new MMSplashScreen(QPixmap(":/splash.png"));
//#endif
    g_splash->showMessage(QCoreApplication::translate("main", "Loading plugins..."), Qt::AlignHCenter | Qt::AlignBottom, Qt::white);
    g_splash->show();


    //清空xml日志文件夹
    QDir logDir = Util::logFolderPath();
    logDir.removeRecursively();

    //copy text filter presets
    copyTextFilterPresetFile();

    QDir appDir(qApp->applicationDirPath());

#if defined(Q_OS_MAC)
    appDir.cdUp();
    appDir.cd("Resources");
#endif

    appDir.cd("share");
    appDir.cd("mlt");
    setenv("MLT_DATA", appDir.path().toUtf8().constData(), 1);


#if defined(Q_OS_MAC)
    resolve_security_bookmark();
#endif

    Registration.readAndCheckRegistrationInfo();

    a.setProperty("system-style", a.style()->objectName());

    QFile file(":/src/qss/mainstylesheet.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QString::fromLatin1(file.readAll());
    qApp->setStyleSheet(styleSheet);

    QFontDatabase::addApplicationFont(":/fonts/DigifaceWide.ttf");

    MainWindow::changeTheme("dark");//Settings.theme());




    a.mainWindow = &MAIN;


    a.mainWindow->createMultitrackModelIfNeeded();

    int screen_width    = QApplication::desktop()->screenGeometry().width();
    int screen_height   = QApplication::desktop()->screenGeometry().height();
    int appWindowWidth  = a.mainWindow->width();
    int appWindowHeight = a.mainWindow->height();
    int origin_x        = screen_width/2    - appWindowWidth/2;
    int origin_y        = screen_height/2   - appWindowHeight/2;

    a.mainWindow->setGeometry(origin_x, origin_y, appWindowWidth, appWindowHeight);
    a.mainWindow->show();
//    a.mainWindow->move ((QApplication::desktop()->width() - a.mainWindow->width())/2,(QApplication::desktop()->height() - a.mainWindow->height())/2);

    a.mainWindow->setFullScreen(a.isFullScreen);


    g_splash->finish(a.mainWindow);
    delete g_splash;
    g_splash = nullptr;

    if (!a.resourceArg.isEmpty())
        a.mainWindow->open(a.resourceArg);
    else
        a.mainWindow->open(a.mainWindow->untitledFileName());




//#ifdef MOVIEMATOR_FREE
////    if (Settings.firstUse())
////    {
////        a.mainWindow->showUpgradeToProPromptDialog();
////    }

//    QDateTime lastUse = Settings.lastUse();
//    QDateTime currentTime = QDateTime::currentDateTime();
//    if (lastUse.isNull())
//    {
//        a.mainWindow->showUpgradeToProPromptDialog();
//    }
//    else
//    {

//        int days = lastUse.daysTo(currentTime);
//        if (days > 1)
//        {
//            a.mainWindow->showUpgradeToProPromptDialog();
//        }
//    }
//    Settings.setLastUse(currentTime);
//#endif


//    MAINCONTROLLER.initPythonQt();
//    PythonQtObjectPtr mainContext = PythonQt::self()->getMainModule();
//    PythonQtScriptingConsole console(NULL, mainContext);
//    console.show();

    int result = a.exec();

    if (EXIT_RESTART == result) {
        LOG_DEBUG() << "restarting app";
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
        ::qputenv("LIBGL_ALWAYS_SOFTWARE",
            Settings.drawMethod() == Qt::AA_UseSoftwareOpenGL && !Settings.playerGPU()
            ? "1" : "0");
#endif
        QProcess* restart = new QProcess;
        QStringList args = a.arguments();
        if (!args.isEmpty())
            args.removeFirst();
        restart->start(a.applicationFilePath(), args, QIODevice::NotOpen);
        result = EXIT_SUCCESS;
}

    return result;
}
