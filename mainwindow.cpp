#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->previousButton->setVisible(false);

    QFile f_timeZones(":/data/data/timezones");

    if (!f_timeZones.open(QIODevice::ReadOnly))
        qDebug() << f_timeZones.errorString();

    QTextStream stream_tz(&f_timeZones);
    m_timeZones.clear();

    while (!stream_tz.atEnd()) {
        QString line(stream_tz.readLine());
        QStringList split = line.split(':');
        ui->timeZoneListWidget->addItem(split.at(0));
        m_timeZones << line;
    }

    f_timeZones.close();

    QFile f_langPacks(":/data/data/langpacks");

    if (!f_langPacks.open(QIODevice::ReadOnly))
        qDebug() << f_langPacks.errorString();

    QTextStream stream_lp(&f_langPacks);
    m_langPacks.clear();

    while (!stream_lp.atEnd()) {
        QString line(stream_lp.readLine());
        QStringList split = line.split(':');
        ui->langPackListWidget->addItem(split.at(1));
        m_langPacks << line;
    }

    f_langPacks.close();

    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

    ui->frame->setStyleSheet(QString("background-color: black;"));

    colorizedLogoPixmap = new ColorizedPixmap(this);
    colorizedLogoPixmap->setFixedSize(666, 100);
    ui->horizontalLayout_3->removeWidget(ui->logo);
    ui->logo->hide();
    ui->horizontalLayout_3->removeItem(ui->horizontalSpacer);
    ui->horizontalLayout_3->addWidget(colorizedLogoPixmap);
    QSpacerItem *logoEndSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->horizontalLayout_3->addSpacerItem(logoEndSpacer);
    colorizedLogoPixmap->repaint();

    ui->previousPageLabel->setVisible(false);
    ui->previousButton->setVisible(false);

    installationHandler = new InstallationHandler();

    this->setMaximumSize(777, 444);

    m_currentPage = 0;

    QObject::connect(ui->timeZoneListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(timeZoneClicked()));
    QObject::connect(ui->langPackListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(langPackClicked()));
    QObject::connect(ui->preparePartitionsButton, SIGNAL(clicked()), this, SLOT(preparePartitionsButtonClicked()));
    QObject::connect(ui->destDriveCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(deviceComboTextChanged(QString)));
    QObject::connect(ui->colorButton, SIGNAL(clicked()), this, SLOT(colorButtonClicked()));
    QObject::connect(ui->previousButton, SIGNAL(clicked()), this, SLOT(previousButtonClicked()));
    QObject::connect(ui->continueButton, SIGNAL(clicked()), this, SLOT(continueButtonClicked()));
    QObject::connect(ui->quitButton, SIGNAL(clicked()), this, SLOT(quitButtonClicked()));
}

void MainWindow::continueButtonClicked()
{
    if (m_currentPage == 0) {
        ui->networkCheckButton->setVisible(false);
        ui->stackedWidget->setCurrentWidget(ui->configurationPage);
        ui->previousPageLabel->setText("<i>Time Zone, Language & Keyboard</i>");
        ui->previousPageLabel->setVisible(true);
        ui->previousButton->setVisible(true);
        ui->previousButton->repaint();
        ui->continueButton->setEnabled(false);
        ui->continueButton->repaint();

        m_currentPage = 1;

        ui->networkStatusProgress->setValue(33);

        QProcess *p = new QProcess;
        p->start("nmcli", QStringList() << "dev" << "wifi");

        if (!p->waitForStarted()) {

        }

        ui->networkStatusProgress->setValue(66);

        if (!p->waitForFinished()) {

        }

        ui->networkStatusProgress->setValue(100);
        ui->previousButton->setEnabled(true);
        ui->previousButton->repaint();

        QString out = p->readAllStandardOutput();

        bool connected = false;
        QString networkName = "";
        if (out.contains("\n"))
        {
            for (QString l : out.split("\n")) {
                qDebug () << l;
                if (l.contains("*")) {
                    connected = true;
                    l.replace("  ", " ");
                    networkName = l.split(" ").at(5);
                }
            }
        }

        if (connected) {
            ui->networkStatusLabel->setText("Network found: " + networkName);
            ui->networkStatusLabel->setVisible(false);
            ui->networkStatusProgress->setValue(100);
            ui->networkStatusProgress->setVisible(false);
            ui->networkCheckButton->setVisible(false);
            ui->hostnameLine->setFocus();
        } else {
            ui->networkStatusLabel->setText("No network found.");
            ui->networkStatusProgress->setValue(0);
            ui->networkCheckButton->setVisible(true);

            connect(ui->networkCheckButton, SIGNAL(clicked()), this, SLOT(networkCheckClicked()));
        }

        connect(ui->hostnameLine, SIGNAL(textChanged(QString)), this, SLOT(validateNetworkPage()));
        validateNetworkPage();

    } else if (m_currentPage == 1) {
        ui->stackedWidget_2->setCurrentIndex(1);
        ui->previousPageLabel->setText("<i>Network Configuration</i>");
        ui->continueButton->setEnabled(false);
        ui->userPageButton->setEnabled(true);
        ui->userDisplayNameLine->setFocus();
        m_currentPage = 2;

        connect(ui->userDisplayNameLine, SIGNAL(textChanged(QString)), this, SLOT(validateUserPage()));
        connect(ui->userUserNameLine, SIGNAL(textChanged(QString)), this, SLOT(validateUserPage()));
        connect(ui->userPasswordLine, SIGNAL(textChanged(QString)), this, SLOT(validateUserPage()));
        connect(ui->userConfirmPasswordLine, SIGNAL(textChanged(QString)), this, SLOT(validateUserPage()));
        validateUserPage();

    } else if (m_currentPage == 2) {
        ui->stackedWidget_2->setCurrentIndex(2);
        ui->previousPageLabel->setText("<i>User Account</i>");
        ui->colorPageButton->setEnabled(true);
        m_currentPage = 3;

    } else if (m_currentPage == 3) {
        ui->stackedWidget_2->setCurrentIndex(3);
        ui->previousPageLabel->setText("<i>Color</i>");
        ui->continueButton->setEnabled(false);
        ui->softwarePageButton->setEnabled(true);
        m_currentPage = 4;

        validateSoftwarePage();
    } else if (m_currentPage == 4) {
        ui->stackedWidget_2->setCurrentIndex(4);
        ui->previousPageLabel->setText("<i>Software</i>");
        ui->continueButton->setEnabled(false);
        ui->destinationPageButton->setEnabled(true);
        m_currentPage = 5;

        QProcess lsblkProc;
        lsblkProc.start("lsblk", QStringList());
        lsblkProc.waitForStarted();
        lsblkProc.waitForFinished();
        m_lsblkOutput = lsblkProc.readAllStandardOutput();
        for (QString line : m_lsblkOutput.split("\n")) {
            if (line.contains("disk")) {
                QString deviceItem = line.simplified().split(" ").at(0) + " (" + line.simplified().split(" ").at(3) + ")";
                ui->destDriveCombo->addItem("/dev/" + deviceItem);
            }
        }
        ui->continueButton->setEnabled(true);
    } else if (m_currentPage == 5) {
        if (ui->destDriveCombo->currentText().contains("1.8T") || ui->destDriveCombo->currentText().contains("476.9G")) {
            qDebug() << "WRONG DRIVE";
            QMessageBox::information(this, "Error", "Careful... check your installation drive.");
        }

        ui->stackedWidget_2->setCurrentIndex(5);
        ui->previousPageLabel->setText("<i>Destination</i>");
        ui->continueButton->setEnabled(false);
        ui->progressBar->setValue(0);
        ui->progressBar->setRange(0, 200);
        m_currentPage = 5;

        installationHandler->setRootDevice("/dev/" + ui->destRootCombo->currentText().split(": ").at(1));
        installationHandler->setBootDevice("/dev/" + ui->destBootCombo->currentText().split(": ").at(1));
        installationHandler->init(this);
        installationHandler->installSystem();
        // todo: review selections and do actual installation here

        QTimer *timer = new QTimer(this);
        timer->setInterval(1000);
        connect(timer, &QTimer::timeout, this, [=](){
            qDebug() << ui->progressBar->value();
            ui->progressBar->setValue(ui->progressBar->value() + 1);
            ui->progressTextLabel->setText("Installing System...");
        });
        timer->start();
    }
}

void MainWindow::previousButtonClicked()
{
    if (m_currentPage == 1) {
        m_currentPage = 0;
        ui->stackedWidget->setCurrentIndex(0);
        ui->previousPageLabel->setVisible(false);
        ui->previousButton->setVisible(false);
        ui->continueButton->setEnabled(true);
    } else if (m_currentPage == 2) {
        m_currentPage = 1;
        ui->previousPageLabel->setText("<i>Time Zone, Language & Keyboard</i>");
        ui->stackedWidget->setCurrentIndex(1);
        ui->stackedWidget_2->setCurrentIndex(0);
        validateNetworkPage();
    } else if (m_currentPage == 3) {
        m_currentPage = 2;
        ui->previousPageLabel->setText("<i>Network Configuration</i>");
        ui->stackedWidget->setCurrentIndex(1);
        ui->stackedWidget_2->setCurrentIndex(1);
        validateUserPage();
    } else if (m_currentPage == 4) {
        m_currentPage = 3;
        ui->previousPageLabel->setText("<i>User Account</i>");
        ui->stackedWidget->setCurrentIndex(1);
        ui->stackedWidget_2->setCurrentIndex(2);
    } else if (m_currentPage == 5) {
        m_currentPage = 4;
        ui->previousPageLabel->setText("<i>Color</i>");
        ui->stackedWidget->setCurrentIndex(1);
        ui->stackedWidget_2->setCurrentIndex(3);
        validateSoftwarePage();
    } else if (m_currentPage == 6) {
// ??
    }
}

void MainWindow::quitButtonClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->configurationPage);
    ui->stackedWidget_2->setCurrentIndex(3);
    m_currentPage = 4;
    continueButtonClicked();
//    this->close();
}

void MainWindow::preparePartitionsButtonClicked()
{
    qDebug() << "A";
    QProcess *p = new QProcess(this);
    connect(p, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(preparePartitionsProcessFinished(int, QProcess::ExitStatus)));
    p->startDetached("partitionmanager", QStringList());
    qDebug() << "B";
}

void MainWindow::preparePartitionsProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << exitCode;
    m_currentPage = 4;
    ui->destDriveCombo->clear();
    continueButtonClicked();
        qDebug() << "C";
}

void MainWindow::colorButtonClicked()
{
    colorChooser = new ColorChooser();
    colorChooser->setWindowState(Qt::WindowFullScreen);
    colorChooser->colorButton = ui->colorButton;
    colorChooser->logoLabel = colorizedLogoPixmap;
    colorChooser->installerWidget = ui->centralwidget;
    colorChooser->show();
}

void MainWindow::networkCheckClicked()
{
    ui->networkCheckButton->setVisible(false);
    ui->networkStatusLabel->setText("Searching for network...");
    ui->networkStatusProgress->setValue(33);

    QProcess *p = new QProcess;
    p->start("nmcli", QStringList() << "dev" << "wifi");
    p->waitForStarted();
    ui->networkStatusProgress->setValue(66);
    p->waitForFinished();
    ui->networkStatusProgress->setValue(100);

    QString out = p->readAllStandardOutput();
    bool connected = false;
    QString networkName = "";
    if (out.contains("\n"))
    {
        for (QString l : out.split("\n")) {
            qDebug () << l;
            if (l.contains("*")) {
                connected = true;
                l.replace("  ", " ");
                networkName = l.split(" ").at(5);
            }
        }
    }

    if (connected) {
        ui->networkStatusLabel->setText("Network found: " + networkName);
        ui->networkStatusProgress->setValue(100);
        ui->networkStatusProgress->setVisible(false);
        ui->networkCheckButton->setVisible(false);
        ui->hostnameLine->setFocus();
    } else {
        ui->networkStatusLabel->setText("No network found.");
        ui->networkStatusProgress->setValue(0);
        ui->networkCheckButton->setVisible(true);
    }
}

void MainWindow::timeZoneClicked()
{
    QString currentTimeZoneData = m_timeZones.at(ui->timeZoneListWidget->currentRow());

    if (currentTimeZoneData.split(":").count() > 1) {
        ui->langPackListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->langPackListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

        if (currentTimeZoneData.split(":").at(2).contains(",")) {
            QString lang = currentTimeZoneData.split(":").at(2).split(",").at(0);
            int row = 0;
            for (QString langPack : m_langPacks)
            {
                if (lang == langPack.split(":").at(0)) {
                    ui->langPackListWidget->setCurrentRow(row, QItemSelectionModel::Current);
                    ui->langPackListWidget->setCurrentIndex(ui->langPackListWidget->model()->index(row, 0));
                }

                row++;
            }
        }
        else {
            QString lang = currentTimeZoneData.split(":").at(2).split(",").at(0);
            int row = 0;
            for (QString langPack : m_langPacks)
            {
                if (lang == langPack.split(":").at(0)) {
                    ui->langPackListWidget->setCurrentRow(row);
                    ui->langPackListWidget->setCurrentIndex(ui->langPackListWidget->model()->index(row, 0));
                }

                row++;
            }
        }
    }

    QDateTime dt = QDateTime::currentDateTime();
    QString tz = currentTimeZoneData.split(":").at(0);
    QByteArray tzBA = tz.toUtf8();
    dt.setTimeZone(QTimeZone(tzBA));
    ui->dateTimeEdit->setDateTime(dt);

    ui->continueButton->setEnabled(true);
}

void MainWindow::langPackClicked()
{
    QString currentLangPackData = m_langPacks.at(ui->langPackListWidget->currentRow());
    ui->keymapButton->setText(currentLangPackData.split(":").at(0));
    ui->continueButton->setEnabled(true);
}

void MainWindow::validateNetworkPage()
{
    if (ui->hostnameLine->text().length() >= 1) {
        // if (ui->networkStatusLabel->text().startsWith("Network found")) {
            ui->continueButton->setEnabled(true);
            ui->hostnameLineValidIcon->setVisible(false);
        // }
    } else {
        ui->hostnameLineValidIcon->setVisible(true);
        ui->continueButton->setEnabled(false);
    }
}

void MainWindow::validateUserPage()
{
    if (ui->userUserNameLine->text().length() >= 1) {
        if (ui->userPasswordLine->text().length() >= 1) {
            if (ui->userConfirmPasswordLine->text().length() >= 1) {
                if (ui->userPasswordLine->text() == ui->userConfirmPasswordLine->text()) {
                    qDebug() << "valid user";
                    ui->continueButton->setEnabled(true);
                } else {
                    qDebug() << "passwords don't match";
                    ui->continueButton->setEnabled(false);
                }
            } else {
                qDebug() << "blank confirm password";
                ui->continueButton->setEnabled(false);
            }
        } else {
            qDebug() << "blank password";
            ui->continueButton->setEnabled(false);
        }
    } else {
        qDebug() << "blank username";
        ui->continueButton->setEnabled(false);
    }
}

void MainWindow::validateSoftwarePage()
{
    ui->continueButton->setEnabled(true);
}

void MainWindow::deviceComboTextChanged(QString text)
{
    QObject::disconnect(ui->destRootCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(destRootComboTextChanged(QString)));
    QObject::disconnect(ui->destBootCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(destBootComboTextChanged(QString)));
    QObject::disconnect(ui->destSwapCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(destSwapComboTextChanged(QString)));
    ui->destRootCombo->clear();
    ui->destRootCombo->addItem("Select [root] partition...");
    ui->destBootCombo->clear();
    ui->destBootCombo->addItem("Select [boot] partition...");
    ui->destSwapCombo->clear();
    ui->destSwapCombo->addItem("Select [swap] partition...");

    QString deviceName = text.split("/").at(2).split(" ").at(0);
    QProcess lsblkProc;
    QStringList args;
    args << QString("/dev/" + deviceName) << "-o" << "NAME,SIZE,TYPE,FSTYPE";
    lsblkProc.start("lsblk", args);
    lsblkProc.waitForStarted();
    lsblkProc.waitForFinished();
    QString output = lsblkProc.readAllStandardOutput();
    for (QString line : output.split("\n")) {
        if (line.contains(deviceName)) {
            if (line.contains("part")) {
                QString itemText = line.simplified().split("─").at(1);
                QStringList itemTextList = itemText.split(" ");
                if (itemTextList.length() > 3) {
                    itemText = itemTextList[1] + " " + itemTextList[3].toUpper() + " Partition at: " + itemTextList[0];
                } else {
                    itemText = itemTextList[1] + " Partition at: " + itemTextList[0];
                }
                ui->destRootCombo->addItem(itemText);
                ui->destRootCombo->setCurrentIndex(0);
                ui->destBootCombo->addItem(itemText);
                ui->destBootCombo->setCurrentIndex(0);
                ui->destSwapCombo->addItem(itemText);
                ui->destSwapCombo->setCurrentIndex(0);
            }
        }
    }

    QObject::connect(ui->destRootCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(destRootComboTextChanged(QString)));
    QObject::connect(ui->destBootCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(destBootComboTextChanged(QString)));
    QObject::connect(ui->destSwapCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(destSwapComboTextChanged(QString)));
}

double MainWindow::convertHumanSizeToBytes(QString text)
{
    double mb = 0;
    QString size = text.split(" ").at(0);
    if (size.contains("T")) {
        mb = size.split("T").at(0).toDouble() * 1024 * 1024 * 1024;
    } else if (size.contains("G")) {
        mb = size.split("G").at(0).toDouble() * 1024 * 1024;
    } else if (size.contains("M")) {
        mb = size.split("M").at(0).toDouble() * 1024;
    } else {
        mb = size.split("K").at(0).toDouble();
    }

    return mb;
}

void MainWindow::destRootComboTextChanged(QString text)
{
    if (convertHumanSizeToBytes(text) < 10000000) {
        QMessageBox::information(this, "[root] size", "/ must be at least 10GB");
    } else {
        if (text.split(" ").at(1) == "EXT4") {
            currentRootPartValid = true;
        } else {
            QMessageBox::information(this, "[root] filesystem", "/ must be formatted as EXT4, BTRFS or ZFS");
            currentRootPartValid = false;
        }
    }

    validatePartitionPage();
}

void MainWindow::destBootComboTextChanged(QString text)
{
    if (convertHumanSizeToBytes(text) < 100000) {
        QMessageBox::information(this, "[boot] size", "/boot must be at least 100MB");
    } else {
        if (text.split(" ").at(1) == "VFAT") {
            currentBootPartValid = true;
        } else {
            QMessageBox::information(this, "[boot] filesystem", "EFI/[boot] must be formatted as FAT32");
            currentBootPartValid = false;
        }
    }

    validatePartitionPage();
}

void MainWindow::destSwapComboTextChanged(QString text)
{
    if (convertHumanSizeToBytes(text) < 50000) {
        QMessageBox::information(this, "[swap] size", "swap must be at least 50MB");
        currentSwapPartValid = false;
    } else {
        if (text.split(" ").at(1) == "SWAP") {
            currentSwapPartValid = true;
        } else {
            QMessageBox::information(this, "[swap] filesystem", "[swap] must be formatted as Linux Swap");
            currentSwapPartValid = false;
        }
    }

    validatePartitionPage();
}

void MainWindow::validatePartitionPage()
{
    if (currentRootPartValid && currentBootPartValid && currentSwapPartValid) {
        ui->continueButton->setEnabled(true);
    } else {
        ui->continueButton->setEnabled(false);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
