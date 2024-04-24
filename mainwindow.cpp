#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->previousButton->setVisible(false);

    QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/images/start-here-system-red.png")));

    QFile f_timeZones(QStringLiteral(":/data/data/timezones"));

    if (!f_timeZones.open(QIODevice::ReadOnly))
        qDebug() << f_timeZones.errorString();

    QTextStream stream_tz(&f_timeZones);
    m_timeZones.clear();

    while (!stream_tz.atEnd()) {
        QString line(stream_tz.readLine());
        QStringList split = line.split(QStringLiteral(":"));
        ui->timeZoneListWidget->addItem(split.at(0));
        m_timeZones << line;
    }

    ui->timeZoneListWidget->setCurrentRow(0);
    f_timeZones.close();

    QFile f_langPacks(QStringLiteral(":/data/data/langpacks"));

    if (!f_langPacks.open(QIODevice::ReadOnly))
        qDebug() << f_langPacks.errorString();

    QTextStream stream_lp(&f_langPacks);
    m_langPacks.clear();

    while (!stream_lp.atEnd()) {
        QString line(stream_lp.readLine());
        QStringList split = line.split(QStringLiteral(":"));
        ui->langPackListWidget->addItem(split.at(1));
        m_langPacks << line;
    }

    ui->langPackListWidget->setCurrentRow(0);
    f_langPacks.close();

    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

    setMouseTracking(true);

    ui->frame->setStyleSheet(QStringLiteral("background-color: black;"));

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

    this->setMaximumSize(777, 544);

    m_currentPage = 0;

    QObject::connect(ui->timeZoneListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(timeZoneClicked()));
    QObject::connect(ui->langPackListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(langPackClicked()));
    QObject::connect(ui->keymapButton, SIGNAL(clicked()), this, SLOT(keymapClicked()));
    QObject::connect(ui->preparePartitionsButton, SIGNAL(clicked()), this, SLOT(preparePartitionsButtonClicked()));
    QObject::connect(ui->destDriveCombo, SIGNAL(currentTextChanged(QString)), this, SLOT(deviceComboTextChanged(QString)));
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
        ui->previousPageLabel->setText(QStringLiteral("<i>Time Zone, Language & Keyboard</i>"));
        ui->previousPageLabel->setVisible(true);
        ui->previousButton->setVisible(true);
        ui->previousButton->repaint();
        ui->continueButton->setEnabled(false);
        ui->continueButton->repaint();

        m_currentPage = 1;

        ui->networkStatusProgress->setValue(33);

        QProcess *p = new QProcess;
        p->start(QStringLiteral("nmcli"), QStringList() << QStringLiteral("dev") << QStringLiteral("wifi"));

        if (!p->waitForStarted()) {

        }

        ui->networkStatusProgress->setValue(66);

        if (!p->waitForFinished()) {

        }

        ui->networkStatusProgress->setValue(100);
        ui->previousButton->setEnabled(true);
        ui->previousButton->repaint();

        QString out = QString::fromUtf8(p->readAllStandardOutput());

        // bool connected = false;
        // QString networkName = QStringLiteral("");
        // if (out.contains(QStringLiteral("\n")))
        // {
        //     for (QString l : out.split(QStringLiteral("\n"))) {
        //         qDebug () << l;
        //         if (l.contains(QStringLiteral("*"))) {
        //             connected = true;
        //             l.replace(QStringLiteral("  "), QStringLiteral(" "));
        //             networkName = l.split(QStringLiteral(" ")).at(5);
        //         }
        //     }
        // }
        //
        // if (connected) {
        //     ui->networkStatusLabel->setText(QStringLiteral("Network found: ") + networkName);
        //     ui->networkStatusLabel->setVisible(false);
        //     ui->networkStatusProgress->setValue(100);
        //     ui->networkStatusProgress->setVisible(false);
        //     ui->networkCheckButton->setVisible(false);
        //     ui->hostnameLine->setFocus();
        // } else {
        //     ui->networkStatusLabel->setText(QStringLiteral("No network found."));
        //     ui->networkStatusProgress->setValue(0);
        //     ui->networkCheckButton->setVisible(true);
        //
        //     connect(ui->networkCheckButton, SIGNAL(clicked()), this, SLOT(networkCheckClicked()));
        // }

        connect(ui->hostnameLine, SIGNAL(textChanged(QString)), this, SLOT(validateNetworkPage()));
        validateNetworkPage();

    } else if (m_currentPage == 1) {
        ui->stackedWidget_2->setCurrentIndex(1);
        ui->previousPageLabel->setText(QStringLiteral("<i>Network Configuration</i>"));
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
        ui->previousPageLabel->setText(QStringLiteral("<i>User Account</i>"));
        ui->colorPageButton->setEnabled(true);
        m_currentPage = 3;

    } else if (m_currentPage == 3) {
        ui->stackedWidget_2->setCurrentIndex(3);
        ui->previousPageLabel->setText(QStringLiteral("<i>Color</i>"));
        ui->continueButton->setEnabled(false);
        ui->softwarePageButton->setEnabled(true);
        m_currentPage = 4;

        validateSoftwarePage();
    } else if (m_currentPage == 4) {
        ui->stackedWidget_2->setCurrentIndex(4);
        ui->previousPageLabel->setText(QStringLiteral("<i>Software</i>"));
        ui->continueButton->setEnabled(false);
        ui->destinationPageButton->setEnabled(true);
        m_currentPage = 5;

        QProcess lsblkProc;
        lsblkProc.start(QStringLiteral("lsblk"), QStringList());
        lsblkProc.waitForStarted();
        lsblkProc.waitForFinished();
        m_lsblkOutput = QString::fromUtf8(lsblkProc.readAllStandardOutput());
        for (QString line : m_lsblkOutput.split(QChar::fromLatin1('\n'))) {
            if (line.contains(QStringLiteral("disk"))) {
                QString deviceItem = line.simplified().split(QStringLiteral(" ")).at(0) + QStringLiteral(" (") + line.simplified().split(QStringLiteral(" ")).at(3) + QStringLiteral(")");
                ui->destDriveCombo->addItem(QStringLiteral("/dev/") + deviceItem);
            }
        }
        ui->continueButton->setEnabled(true);
    } else if (m_currentPage == 5) {
        if (ui->destDriveCombo->currentText().contains(QStringLiteral("1.8T")) || ui->destDriveCombo->currentText().contains(QStringLiteral("476.9G"))) {
            qDebug() << "WRONG DRIVE";
            QMessageBox::information(this, QString::fromUtf8("Error"), QString::fromUtf8("Careful... check your installation drive."));
        }

        ui->stackedWidget_2->setCurrentIndex(5);
        ui->previousPageLabel->setText(QStringLiteral("<i>Destination</i>"));
        ui->continueButton->setEnabled(false);
        ui->progressBar->setValue(0);
        ui->progressBar->setRange(0, 200);
        m_currentPage = 5;

        installationHandler->setRootDevice(QStringLiteral("/dev/") + ui->destRootCombo->currentText().split(QStringLiteral(": ")).at(1));
        installationHandler->setBootDevice(QStringLiteral("/dev/") + ui->destBootCombo->currentText().split(QStringLiteral(": ")).at(1));
        installationHandler->init(this);
        installationHandler->installSystem();
        // todo: review selections and do actual installation here

        QTimer *timer = new QTimer(this);
        timer->setInterval(1000);
        connect(timer, &QTimer::timeout, this, [=, this](){
            qDebug() << ui->progressBar->value();
            ui->progressBar->setValue(ui->progressBar->value() + 1);
            ui->progressTextLabel->setText(QStringLiteral("Installing System..."));
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
        ui->previousPageLabel->setText(QStringLiteral("<i>Time Zone, Language & Keyboard</i>"));
        ui->stackedWidget->setCurrentIndex(1);
        ui->stackedWidget_2->setCurrentIndex(0);
        validateNetworkPage();
    } else if (m_currentPage == 3) {
        m_currentPage = 2;
        ui->previousPageLabel->setText(QStringLiteral("<i>Network Configuration</i>"));
        ui->stackedWidget->setCurrentIndex(1);
        ui->stackedWidget_2->setCurrentIndex(1);
        validateUserPage();
    } else if (m_currentPage == 4) {
        m_currentPage = 3;
        ui->previousPageLabel->setText(QStringLiteral("<i>User Account</i>"));
        ui->stackedWidget->setCurrentIndex(1);
        ui->stackedWidget_2->setCurrentIndex(2);
    } else if (m_currentPage == 5) {
        m_currentPage = 4;
        ui->previousPageLabel->setText(QStringLiteral("<i>Color</i>"));
        ui->stackedWidget->setCurrentIndex(1);
        ui->stackedWidget_2->setCurrentIndex(3);
        validateSoftwarePage();
    } else if (m_currentPage == 6) {
// ??
    }
}

void MainWindow::quitButtonClicked()
{
    // ui->stackedWidget->setCurrentWidget(ui->configurationPage);
    // ui->stackedWidget_2->setCurrentIndex(3);
    // m_currentPage = 4;
    // continueButtonClicked();
    this->close();
}

void MainWindow::keymapClicked()
{
    localeMenu = new QMenu(this);

    QString currentTimeZoneData = m_timeZones.at(ui->timeZoneListWidget->currentRow());
    QStringList locales = currentTimeZoneData.split(QStringLiteral(":")).at(1).split(QStringLiteral(","));
    QStringListIterator i(locales);
    while (i.hasNext()) {
        QString locale = i.next();

        if (locale.contains(QStringLiteral("."))) {
            QAction *act = new QAction(this);
            act->setText(locale);
            act->setData(locale);
            localeMenu->addAction(act);
        }
    }

    localeMenu->popup(QCursor::pos());

    connect(localeMenu, &QMenu::triggered, [this] (QAction* a) {
        ui->keymapButton->setText(a->data().toString());
    });

    // ui->keymapButton->setText(currentLangPackData.split(QStringLiteral(":")).at(0));
}

void MainWindow::preparePartitionsButtonClicked()
{
    QProcess *p = new QProcess(this);
    connect(p, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(preparePartitionsProcessFinished(int, QProcess::ExitStatus)));
    p->startDetached(QStringLiteral("partitionmanager"), QStringList());
}

void MainWindow::preparePartitionsProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << exitCode;
    m_currentPage = 4;
    ui->destDriveCombo->clear();
    continueButtonClicked();
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
    ui->networkStatusLabel->setText(QStringLiteral("Searching for network..."));
    ui->networkStatusProgress->setValue(33);

    QProcess *p = new QProcess;
    p->start(QStringLiteral("nmcli"), QStringList() << QStringLiteral("dev") << QStringLiteral("wifi"));
    p->waitForStarted();
    ui->networkStatusProgress->setValue(66);
    p->waitForFinished();
    ui->networkStatusProgress->setValue(100);

    QString out = QString::fromUtf8(p->readAllStandardOutput());
    bool connected = false;
    QString networkName = QStringLiteral("");
    if (out.contains(QStringLiteral("\n")))
    {
        for (QString l : out.split(QString::fromUtf8("\n"))) {
            qDebug () << l;
            if (l.contains(QStringLiteral("*"))) {
                connected = true;
                l.replace(QStringLiteral("  "), QStringLiteral(" "));
                networkName = l.split(QStringLiteral(" ")).at(5);
            }
        }
    }

    if (connected) {
        ui->networkStatusLabel->setText(QStringLiteral("Network found: ") + networkName);
        ui->networkStatusProgress->setValue(100);
        ui->networkStatusProgress->setVisible(false);
        ui->networkCheckButton->setVisible(false);
        ui->hostnameLine->setFocus();
    } else {
        ui->networkStatusLabel->setText(QStringLiteral("No network found."));
        ui->networkStatusProgress->setValue(0);
        ui->networkCheckButton->setVisible(true);
    }
}

void MainWindow::timeZoneClicked()
{
    QString currentTimeZoneData = m_timeZones.at(ui->timeZoneListWidget->currentRow());

    if (currentTimeZoneData.split(QStringLiteral(":")).count() > 1) {
        ui->langPackListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->langPackListWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

        if (currentTimeZoneData.split(QStringLiteral(":")).at(2).contains(QStringLiteral(","))) {
            QString lang = currentTimeZoneData.split(QStringLiteral(":")).at(2).split(QStringLiteral(",")).at(0);
            int row = 0;
            for (QString langPack : m_langPacks)
            {
                if (lang == langPack.split(QStringLiteral(":")).at(0)) {
                    ui->langPackListWidget->setCurrentRow(row, QItemSelectionModel::Current);
                    ui->langPackListWidget->setCurrentIndex(ui->langPackListWidget->model()->index(row, 0));
                }

                row++;
            }
        }
        else {
            QString lang = currentTimeZoneData.split(QStringLiteral(":")).at(2).split(QStringLiteral(",")).at(0);
            int row = 0;
            for (QString langPack : m_langPacks)
            {
                if (lang == langPack.split(QStringLiteral(":")).at(0)) {
                    ui->langPackListWidget->setCurrentRow(row);
                    ui->langPackListWidget->setCurrentIndex(ui->langPackListWidget->model()->index(row, 0));
                }

                row++;
            }
        }
    }

    QDateTime dt = QDateTime::currentDateTime();
    QString tz = currentTimeZoneData.split(QStringLiteral(":")).at(0);
    QByteArray tzBA = tz.toUtf8();
    dt.setTimeZone(QTimeZone(tzBA));
    ui->dateTimeEdit->setDateTime(dt);

    ui->continueButton->setEnabled(true);
}

void MainWindow::langPackClicked()
{
    QString currentLangPackData = m_langPacks.at(ui->langPackListWidget->currentRow());
    ui->keymapButton->setText(currentLangPackData.split(QStringLiteral(":")).at(0));
    ui->continueButton->setEnabled(true);
}

void MainWindow::validateNetworkPage()
{
    if (ui->hostnameLine->text().length() >= 1) {
        // if (ui->networkStatusLabel->text().startsWith(QStringLiteral("Network found")) {
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
    ui->destRootCombo->addItem(QStringLiteral("Select [root] partition..."));
    ui->destBootCombo->clear();
    ui->destBootCombo->addItem(QStringLiteral("Select [boot] partition..."));
    ui->destSwapCombo->clear();
    ui->destSwapCombo->addItem(QStringLiteral("Select [swap] partition..."));

    QString deviceName = text.split(QStringLiteral("/")).at(2).split(QStringLiteral(" ")).at(0);
    QProcess lsblkProc;
    QStringList args;
    args << QString(QStringLiteral("/dev/") + deviceName) << QStringLiteral("-o") << QStringLiteral("NAME,SIZE,TYPE,FSTYPE");
    lsblkProc.start(QStringLiteral("lsblk"), args);
    lsblkProc.waitForStarted();
    lsblkProc.waitForFinished();
    QString output = QString::fromUtf8(lsblkProc.readAllStandardOutput());
    for (QString line : output.split(QStringLiteral("\n"))) {
        if (line.contains(deviceName)) {
            if (line.contains(QStringLiteral("part"))) {
                QString itemText = line.simplified().split(QStringLiteral("─")).at(1);
                QStringList itemTextList = QStringList(itemText.split(QStringLiteral(" ")));
                if (itemTextList.length() > 3) {
                    itemText = itemTextList[1] + QStringLiteral(" ") + itemTextList[3].toUpper() + QString::fromUtf8(" Partition at: ") + itemTextList[0];
                } else {
                    itemText = itemTextList[1] + QString::fromUtf8(" Partition at: ") + itemTextList[0];
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
    QString size = text.split(QStringLiteral(" ")).at(0);
    if (size.contains(QStringLiteral("T"))) {
        mb = size.split(QStringLiteral("T")).at(0).toDouble() * 1024 * 1024 * 1024;
    } else if (size.contains(QStringLiteral("G"))) {
        mb = size.split(QStringLiteral("G")).at(0).toDouble() * 1024 * 1024;
    } else if (size.contains(QStringLiteral("M"))) {
        mb = size.split(QStringLiteral("M")).at(0).toDouble() * 1024;
    } else {
        mb = size.split(QStringLiteral("K")).at(0).toDouble();
    }

    return mb;
}

void MainWindow::destRootComboTextChanged(QString text)
{
    if (convertHumanSizeToBytes(text) < 10000000) {
        QMessageBox::information(this, QString::fromUtf8("[root] size"), QString::fromUtf8("/ must be at least 10GB"));
    } else {
        if (text.split(QStringLiteral(" ")).at(1) == QString::fromUtf8("EXT4")) {
            currentRootPartValid = true;
        } else {
            QMessageBox::information(this, QString::fromUtf8("[root] filesystem"), QString::fromUtf8("/ must be formatted as EXT4, BTRFS or ZFS"));
            currentRootPartValid = false;
        }
    }

    validatePartitionPage();
}

void MainWindow::destBootComboTextChanged(QString text)
{
    if (convertHumanSizeToBytes(text) < 100000) {
        QMessageBox::information(this, QString::fromUtf8("[boot] size"), QString::fromUtf8("/boot must be at least 100MB"));
    } else {
        if (text.split(QStringLiteral(" ")).at(1) == QString::fromUtf8("VFAT")) {
            currentBootPartValid = true;
        } else {
            QMessageBox::information(this, QString::fromUtf8("[boot] filesystem"), QString::fromUtf8("EFI/[boot] must be formatted as FAT32"));
            currentBootPartValid = false;
        }
    }

    validatePartitionPage();
}

void MainWindow::destSwapComboTextChanged(QString text)
{
    if (convertHumanSizeToBytes(text) < 50000) {
        QMessageBox::information(this, QString::fromUtf8("[swap] size"), QString::fromUtf8("swap must be at least 50MB"));
        currentSwapPartValid = false;
    } else {
        if (text.split(QStringLiteral(" ")).at(1) == QString::fromUtf8("SWAP")) {
            currentSwapPartValid = true;
        } else {
            QMessageBox::information(this, QString::fromUtf8("[swap] filesystem"), QString::fromUtf8("[swap] must be formatted as Linux Swap"));
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
