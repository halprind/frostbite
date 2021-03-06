#ifndef GENERICWINDOW_H
#define GENERICWINDOW_H

#include <QObject>
#include <QPlainTextEdit>
#include <QMouseEvent>

#include <generalsettings.h>
#include <mainwindow.h>
#include <windowinterface.h>
#include <snapshot.h>

class MainWindow;
class WindowFacade;
class GeneralSettings;
class Snapshot;

class GenericWindow : public QPlainTextEdit, public WindowInterface {
    Q_OBJECT

public:
    explicit GenericWindow(QWidget *parent = 0);
    ~GenericWindow();

    QColor getBgColor();
    QColor getTextColor();
    QTextDocument* getDocument();
    QString getObjectName();
    QPlainTextEdit* getMainWindow();

    void setAppend(bool);
    bool append();

private:
    void contextMenuEvent(QContextMenuEvent* event);
    void buildContextMenu();
    void loadSettings();

    MainWindow* mainWindow;
    GeneralSettings* settings;
    WindowFacade* wm;

    Snapshot* snapshot;

    QAction* appearanceAct;
    QAction* copyAct;
    QAction* selectAct;
    QAction* saveAct;
    QAction* clearAct;
    QMenu* menu;

    bool _append;

signals:

private slots:
    void copySelected();
    void enableCopy(bool);
    void saveAsHtml();
    void changeAppearance();
    
public slots:

};

#endif // GENERICWINDOW_H
