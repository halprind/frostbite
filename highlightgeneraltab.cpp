#include "highlightgeneraltab.h"

HighlightGeneralTab::HighlightGeneralTab(QObject *parent) : QObject(parent) {
    highlightDialog = (HighlightDialog*)parent;
    settings = HighlightSettings::Instance();
    audioPlayer = AudioPlayer::Instance();

    listWidget = highlightDialog->getGeneralList();
    alertGroup = highlightDialog->getGeneralAlertGroup();
    fileSelect = highlightDialog->getGeneralFileSelect();
    playButton = highlightDialog->getGeneralPlayButton();

    applyButton = highlightDialog->getApplyButton();

    this->loadSettings();
    this->prepareList();
    this->initFileSelect();
    this->initContextMenu();

    listWidget->setStyleSheet("QListWidget {background-color: black;}");

    connect(listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            this, SLOT(itemSelected(QListWidgetItem*, QListWidgetItem*)));
    connect(listWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(listWidgetMenuRequested(const QPoint &)));
    connect(alertGroup, SIGNAL(clicked(bool)), this, SLOT(alertClicked(bool)));
    connect(fileSelect, SIGNAL(activated(const QString&)), this,
            SLOT(fileSelected(const QString&)));    
    connect(playButton, SIGNAL(clicked()), this, SLOT(playSound()));
}

void HighlightGeneralTab::loadSettings() {
    highlightList.insert("roomName", readSettings("roomName", "[Room titles]", QColor(ROOM_NAME_COLOR_HEX)));
    highlightList.insert("gameMessage", readSettings("gameMessage", "Game Messages", QColor(GAME_MESSAGE_COLOR_HEX)));
    highlightList.insert("speech", readSettings("speech", "Someone says, ..", QColor(SPEECH_COLOR_HEX)));
    highlightList.insert("thinking", readSettings("thinking", "Your mind hears someone thinking, .. ", QColor(THINKING_COLOR_HEX)));
}

void HighlightGeneralTab::initContextMenu() {
    menu = new QMenu(listWidget);
    colorAct = new QAction(QIcon(":/images/open.png"), tr("&Change Color..."), listWidget);
    menu->addAction(colorAct);
    connect(colorAct, SIGNAL(triggered()), this, SLOT(colorDialog()));

    editAct = new QAction(QIcon(":/images/open.png"), tr("&Edit..."), listWidget);
    menu->addAction(editAct);
    editAct->setEnabled(false);
}

void HighlightGeneralTab::colorDialog() {
    QColor color = QColorDialog::getColor(listWidget->currentItem()->textColor(), listWidget, tr("Select color"));

    if (color.isValid()) {
        QString currentItemKey = listWidget->currentItem()->data(Qt::UserRole).toString();
        listWidget->currentItem()->setTextColor(color);
        updateSelectedItemColor(listWidget->currentItem());
        highlightList[currentItemKey].insert("color", color);

        registerChange(currentItemKey);
    }
}

void HighlightGeneralTab::listWidgetMenuRequested(const QPoint &point) {
    QPoint globalPos = listWidget->mapToGlobal(point);
    menu->exec(globalPos);
}

void HighlightGeneralTab::initFileSelect() {
    fileSelect->addItems(audioPlayer->getAudioList());
}

void HighlightGeneralTab::playSound() {
    audioPlayer->play(fileSelect->currentText());
}

void HighlightGeneralTab::prepareList() {
    QHash<QString, QHash<QString, QVariant> >::const_iterator i = highlightList.constBegin();
    while (i != highlightList.constEnd()) {
        QListWidgetItem *newItem = new QListWidgetItem(tr(i.value().value("name").toByteArray().data()), listWidget);
        newItem->setData(Qt::UserRole, i.key());
        newItem->setTextColor(i.value().value("color").value<QColor>());
        newItem->setFont(QFont("Fixedsys", 12));
        ++i;
    }
}

QHash<QString, QVariant> HighlightGeneralTab::readSettings(QString id, QString name, QColor color) {
    QString nameSetting = settings->getSingleParameter("GeneralHighlight/" + id + "/name", "").toString();
    QColor colorSetting = settings->getSingleParameter("GeneralHighlight/" + id + "/color", NULL).value<QColor>();
    QString alertSetting = settings->getSingleParameter("GeneralHighlight/" + id + "/alert", "").toString();

    QHash<QString, QVariant> item;
    item.insert("name", name);
    item.insert("color", colorSetting.isValid() ? colorSetting : color);
    item.insert("alert", alertSetting);

    if(nameSetting.isEmpty()) {
        settings->setSingleParameter("GeneralHighlight/" + id + "/name", name);
        settings->setSingleParameter("GeneralHighlight/" + id + "/color", color);
    }
    return item;
}

void HighlightGeneralTab::registerChange(QString currentItemKey) {
    if(!generalChangeList.contains(currentItemKey)) {
        generalChangeList.append(currentItemKey);

        if(!applyButton->isEnabled()) {
            applyButton->setEnabled(true);
        }
    }
}

void HighlightGeneralTab::updateAlertSettings(QString key, QVariant value) {
    QString currentItemKey = listWidget->currentItem()->data(Qt::UserRole).toString();
    QHash<QString, QVariant> itemSettings = highlightList.value(currentItemKey);

    QHash<QString, QVariant> alertSettings = itemSettings.value("alert").toHash();
    alertSettings.insert(key, value);

    highlightList[currentItemKey].insert("alert", alertSettings);

    registerChange(currentItemKey);
}

void HighlightGeneralTab::alertClicked(bool on) {
    updateAlertSettings("enabled", on);
}

void HighlightGeneralTab::fileSelected(const QString& text) {
    updateAlertSettings("file", text);
}

void HighlightGeneralTab::itemSelected(QListWidgetItem *current, QListWidgetItem *previous) {
    /*if(previous) {
        previous->setIcon(QIcon());
    }
    current->setIcon(QIcon(":/nav/images/active/green/e.png"));*/

    /* change highlight color to item color */
    updateSelectedItemColor(current);

    /* enable/disable controls for list item */
    updateControls(current);
}

void HighlightGeneralTab::updateSelectedItemColor(QListWidgetItem *current) {
    if(current != NULL) {
        /* change highlight color to item color */
        QPalette palette = listWidget->palette();
        palette.setColor(QPalette::HighlightedText, current->textColor());
        //palette.setColor(QPalette::Highlight, Qt::transparent);
        listWidget->setPalette(palette);
    }
}

void HighlightGeneralTab::updateControls(QListWidgetItem *current) {
    if(current != NULL) {
        QString key = current->data(Qt::UserRole).toString();
        QHash<QString, QVariant> itemSettings = highlightList.value(key);
        QHash<QString, QVariant> alertSettings = itemSettings.value("alert").toHash();

        if(!alertSettings.isEmpty()) {
            alertGroup->setChecked(alertSettings.value("enabled").toBool());

            int index = fileSelect->findText(alertSettings.value("file").toString());
            if(index != -1) {
                fileSelect->setCurrentIndex(index);
            } else {
                fileSelect->setCurrentIndex(0);
            }
        } else {
            alertGroup->setChecked(false);
            fileSelect->setCurrentIndex(0);
        }
    } else {
        this->clearControls();
    }
}

void HighlightGeneralTab::clearControls() {
    alertGroup->setChecked(false);
    alertGroup->setDisabled(true);
}

void HighlightGeneralTab::saveChanges() {
    if(!generalChangeList.isEmpty()) {
        foreach(QString s, generalChangeList) {
            QHashIterator<QString, QVariant> i(highlightList.value(s));
            while (i.hasNext()) {
                i.next();
                settings->setSingleParameter("GeneralHighlight/" + s + "/" + i.key(), i.value());
            }
        }

        highlightDialog->reloadWindowStyles();
        generalChangeList.clear();
    }
}

void HighlightGeneralTab::cancelChanges() {
    if(!generalChangeList.isEmpty()) {
        listWidget->clear();
        this->loadSettings();
        this->prepareList();

        generalChangeList.clear();
    }
}

HighlightGeneralTab::~HighlightGeneralTab() {
}
