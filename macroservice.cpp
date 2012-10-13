#include "macroservice.h"

MacroService::MacroService(QObject *parent) : QObject(parent) {
    mainWindow = (MainWindow*)parent;
    macroSettings = MacroSettings::Instance();

    macroThread = new MacroThread();

    connect(this, SIGNAL(init(QHash<QString, QStringList>, int)), macroThread, SLOT(init(QHash<QString, QStringList>, int)));
    connect(macroThread, SIGNAL(setText(QString)), this, SLOT(setText(QString)));
    connect(macroThread, SIGNAL(setCursor(int)), this, SLOT(setCursor(int)));
    connect(macroThread, SIGNAL(writeCommand(QString)), this, SLOT(writeCommand(QString)));
}

bool MacroService::execute(QString macroString) {
    if(!macroString.isEmpty()) {
        int time = macroSettings->getParameter("sequence/time", 1000).toInt();
        emit init(this->processCommands(macroString), time);

        if(!macroThread->isRunning()) {
            macroThread->run();
        }
        return true;
    }
    return false;
}

void MacroService::setText(QString text) {
    mainWindow->getCommandLine()->setText(text);
}

void MacroService::setCursor(int pos) {
    mainWindow->getCommandLine()->moveCursor(pos);
}

void MacroService::writeCommand(QString text) {
    mainWindow->getCommandLine()->writeCommand(text);
}

QHash<QString, QStringList> MacroService::processCommands(QString macroString) {
    QHash<QString, QStringList> macro;

    QStringList commands(macroString.split(QRegExp("\\$n|\\$s"), QString::SkipEmptyParts));
    macro.insert("commands", commands);

    QByteArray bytes(macroString.toLocal8Bit());
    for(int i = 0; i < bytes.length(); i++){
        if(bytes.at(i) == '$') {
            if(i < bytes.length()) {
                macro["actions"] << QString(bytes.at(i + 1));
            }
        }
    }
    return macro;
}

MacroService::~MacroService() {
    delete macroThread;
}
