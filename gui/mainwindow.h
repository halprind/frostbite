#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>

#include <windowfacade.h>
#include <tcpclient.h>
#include <toolbar.h>
#include <clientsettings.h>
#include <commandline.h>
#include <menuhandler.h>
#include <scriptservice.h>
#include <timerbar.h>
#include <generalsettings.h>
#include <tray.h>
#include <scriptapiserver.h>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
#include "cleanlooks/qcleanlooksstyle.h"
#endif

#include <QtXml>

QT_BEGIN_NAMESPACE
class QAction;
class QListWidget;
class QMenu;
class QTextEdit;
class QTextBrowser;
class QTextLine;
QT_END_NAMESPACE

namespace Ui {
    class MainWindow;
}

class WindowFacade;
class TcpClient;
class Toolbar;
class GameWindow;
class CommandLine;
class MenuHandler;
class ScriptService;
class TimerBar;
class GeneralSettings;
class Tray;
class ScriptApiServer;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static bool DEBUG;

    void addDockWidgetMainWindow(Qt::DockWidgetArea, QDockWidget*);
    void addWidgetMainLayout(QWidget*);
    void addToolbarAction(QAction*);
    void addToolbarSeparator();
    void addToolbarSpacer(int);
    void addToolbarWidget(QWidget*);
    void insertProfilesMenu(QMenu*);
    void setLogMain(bool);
    void setLogThoughts(bool);
    void setLogConversations(bool);
    void setLogArrivals(bool);
    void setLogDeaths(bool);
    void setLogDebug(bool);
    void setLogAuth(bool);
    void setToolbarAllowedAreas(Qt::ToolBarAreas);
    void connectEnabled(bool);
    void setBackgroundColor(QColor);
    QColor getBackgroundColor();
    void toggleFullScreen();
    void toggleMaximized();
    void updateProfileSettings();
    void openConnectDialog();
    void openConnection(QString host, QString port, QString key);
    void openAppearanceDialog();
    void saveWindow();

    WindowFacade* getWindowFacade();
    Toolbar* getToolbar();
    TcpClient* getTcpClient();
    CommandLine* getCommandLine();
    ScriptService* getScriptService();
    TimerBar* getTimerBar();
    Tray* getTray();

private:
    Ui::MainWindow* ui;
    WindowFacade* windowFacade;
    Toolbar* toolBar;
    TcpClient* tcpClient;
    ClientSettings* settings;
    GeneralSettings* generalSettings;
    CommandLine* cmdLine;
    MenuHandler* menuHandler;
    ScriptService* scriptService;
    ScriptApiServer* scriptApiServer;

    Tray* tray;
    TimerBar* timerBar;
    QReadWriteLock lock;    

    void initSettings();
    void loadClient();
    void appSetup();

protected:
    void closeEvent(QCloseEvent*);

signals:
    void profileChanged();

public slots:
    void setMainTitle(QString);

/*private slots:
    void menuTriggered(QAction*);*/

};

#endif // MAINWINDOW_H
