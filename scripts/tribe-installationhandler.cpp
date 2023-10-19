
/*
 * Copyright (c) 2010              Dario Freddi <drf@chakra-project.org>
 *               2010, 2011, 2012  Drake Justice <djustice@chakra-project.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include <QDebug>
#include <QFile>

#include <unistd.h>

#include <QDir>
#include <QEventLoop>
#include <QFileInfo>
#include <QProcess>
#include <QTimer>

#include <KIO/Job>
#include <KIO/NetAccess>
#include <KLocalizedString>
#include <KGlobal>

#include <tribepartitionmanager/core/operationstack.h>
#include <tribepartitionmanager/core/partition.h>
#include <tribepartitionmanager/fs/filesystem.h>

#include <config-tribe.h>

#include "pmhandler.h"
#include "installationhandler.h"


class InstallationHandlerHelper
{
public:
    InstallationHandlerHelper() : q(0) {}
    ~InstallationHandlerHelper() {
        delete q;
    }
    InstallationHandler *q;
};

K_GLOBAL_STATIC(InstallationHandlerHelper, s_globalInstallationHandler)

InstallationHandler *InstallationHandler::instance()
{
    if (!s_globalInstallationHandler->q) {
        new InstallationHandler;
    }

    return s_globalInstallationHandler->q;
}

InstallationHandler::InstallationHandler(QObject *parent)
        : QObject(parent),
          m_configurePacman(true)
{
    Q_ASSERT(!s_globalInstallationHandler->q);
    s_globalInstallationHandler->q = this;
}

InstallationHandler::~InstallationHandler()
{
    cleanup();
}

qint64 InstallationHandler::minSizeForTarget() const
{
    return m_minSize;
}

void InstallationHandler::init()
{
    eTime.start();

    QDir dir(INSTALLATION_TARGET);

    if (!dir.exists()) {
        if (!dir.mkpath(INSTALLATION_TARGET)) {
            emit errorInstalling(i18n("Tribe was not able to initialize needed path: ") + QString(INSTALLATION_TARGET) + i18n(" , please resolve the issue, and try again."));
        }
    }

    // compute minimum size for target for partitionpage root part size check
    QFile squash(QString(BOOTMNT_POINT) + "/root-image.sqfs");
    m_minSize = squash.size();
    if (m_minSize <= 0) {
        // Set it to ~700MB
        m_minSize = 700000000;
    }

    m_minSize *= 4;
    m_minSize += 500;


    // initialize m_process
    m_process = new QProcess(this);
    QProcessEnvironment env;
    env.insert("LC_ALL", "C");
    m_process->setProcessEnvironment(env);
    m_process->setProcessChannelMode(QProcess::MergedChannels);
}

void InstallationHandler::setHomeBehaviour(HomeAction act)
{
    homeAction = act;
}

void InstallationHandler::setFileHandlingBehaviour(FileHandling fhnd)
{
    fileAction = fhnd;
}

QStringList InstallationHandler::checkExistingHomeDirs()
{
    QDir dir(QString(QString(INSTALLATION_TARGET) + QString("/home/")));

    if (dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::NoSymLinks).isEmpty())
        return QStringList();
    else
        return dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::NoSymLinks);

    return QStringList();
}

int InstallationHandler::antiFlicker()
{
    int retval = eTime.elapsed();

    if (retval > 200) {
        eTime.restart();
    }

    return retval;
}

void InstallationHandler::handleProgress(CurrentAction act, int percentage)
{
    int total;

    if (antiFlicker() < 200) {   // fixme
        return;
    }

    switch (act) {
    case InstallationHandler::DiskPreparation:
        total = (percentage * 10) / 100;
        break;
    case InstallationHandler::SystemInstallation:
        total = ( (percentage * 70) / 100 ) + 10;
        break;
    case InstallationHandler::PostInstall:
        total = percentage + 80;
        break;
    default:
        total = 0;
        break;
    }

    emit streamProgress(total);
}

void InstallationHandler::installSystem()
{
    currAction = DiskPreparation;

    // Ok, first, let's see prepare the hard drives through the partitioner
    PMHandler::instance()->apply();
}

void InstallationHandler::partitionsFormatted()
{
    // If we got here, the mount list is already populated. So let's reset the
    // iterator, give back meaningful progress, and start

    resetIterators();
    streamLabel(i18n("Mounting partitions..."));
    mountNextPartition();
}

void InstallationHandler::copyFiles()
{
    currAction = InstallationHandler::SystemInstallation;

    emit streamLabel(i18n("Preparing installation..."));

    QProcess::execute("killall pacman");
    QProcess::execute("/bin/rm -f /var/lib/pacman/db.lck");

    switch (m_installationType) {
    case InstallationHandler::Iso: {
        connect(m_process, SIGNAL(readyRead()), SLOT(parseUnsquashfsOutput()));
        connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(jobDone(int)));

        qDebug() << "Installing (sqfs decompression) started...";

        QStringList arguments;
        arguments << "-f" << "-d" << INSTALLATION_TARGET << QString("%1/root-image.sqfs").arg(BOOTMNT_POINT);

        emit streamLabel(i18n("Installing the system..."));
        m_process->start("unsquashfs", arguments);
        break;
    }
    case InstallationHandler::NetInst: {
        connect(m_process, SIGNAL(readyRead()), SLOT(parsePacmanOutput()));
        connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(jobDone(int)));
        qDebug() << "Installing (Pacman netinst) started...";

        QString command = QString("%1/netinstall.sh").arg(SCRIPTS_INSTALL_PATH);
        QStringList arguments;
        arguments << INSTALLATION_TARGET << m_packages;

        m_process->start(command, arguments);
        break;
    }
    default:
        break;
    }

}

void InstallationHandler::jobDone(int result)
{
    m_process->deleteLater();

    if (result) {
        qDebug() << m_process->readAll();
        emit errorInstalling(i18n("Error copying files"));
        return;
    } else {
        emit streamLabel(i18n("Removing packages whose license has been declined..."));

        postRemove();
    }
}

void InstallationHandler::parseUnsquashfsOutput()
{
    QString out = m_process->readLine(2048);

    if (out.contains(QChar(']'))) {     // todo: make me pretty
        QString parsed = out.split(QChar(']')).at(1);
        QStringList members = parsed.split(QChar('/'));
        QString firstMem = members.at(0);
        firstMem = firstMem.remove(QChar(' '));
        int one = firstMem.toInt();
        QString secondMem = members.at(1);
        secondMem = secondMem.split(QChar(' ')).at(0);
        int two = secondMem.toInt();
        int percentage = (one * 100) / two;
        handleProgress(currAction, percentage);
    }
}

void InstallationHandler::parsePacmanOutput()
{
    static int totalPackages = -1;
    static int downloadCnt = 0;
    static int installCnt = 0;
    static QString currentPackage;

    QStringList outlist;
    while (m_process->canReadLine()) {
        QString out = m_process->readLine(2048).trimmed();
        qDebug() << out;

        if (out.startsWith("NETINST")) {
            QString msg = out.mid(7).trimmed();
            if (msg.contains("start")) {
                emit streamLabel(i18n("Starting Netinstall..."));
            } else if (msg.contains("install")) {
                emit streamLabel(i18n("Installing packages..."));
            }
        } else if (out.startsWith("Targets")) {
            int start = 9;
            int end = out.indexOf(')', start);
            // search for number starting at offset 8
            if (end != -1) {
                totalPackages = out.mid(start, end - start).toInt();
                emit streamLabel(i18n("Downloading packages..."));
            }
        } else {
            QRegExp rx("(downloading|installing) (.*)\\.\\.\\.");
            if (rx.indexIn(out) != -1) {
                // only take action if it is the first time we see the package
                if (currentPackage != rx.cap(2)) {
                    currentPackage = rx.cap(2);

                    if (rx.cap(1) == "downloading") {
                        if (totalPackages > 0) {
                            ++downloadCnt;
                        }
                    } else {
                        ++installCnt;
                    }
                }
            }
        }
    }


    if (totalPackages > 0) {
        double downloadWeight = 60;
        double installWeight = 40;
        int percentage = (downloadCnt * downloadWeight) / totalPackages + (installCnt * installWeight) / totalPackages;
        handleProgress(currAction, percentage);
    }
}

void InstallationHandler::reconnectJobSlot()
{
    if (m_process) {
        connect(m_process, SIGNAL(readyRead()), SLOT(parseUnsquashfsOutput()));
    }
}

void InstallationHandler::readyPost()
{
    populateCommandParameters();

    qDebug() << " :: postinstall command: \n" << m_postcommand;

    m_postjob = "initialize-target";
    m_postlabel = i18n("Initializing target...");

    postInstall();
}

void InstallationHandler::populateCommandParameters()
{
    qDebug() << " :: System root partition: " << trimDevice(m_mount["/"]);

    m_postcommand = QString("--target-root %1 --mountpoint %2 ")
                    .arg(trimDevice(m_mount["/"])).arg(INSTALLATION_TARGET);

    if (m_mount.contains("swap")) {
        m_postcommand.append(QString("--use-swap yes --target-swap %1 ").arg(trimDevice(m_mount["swap"])));
    }

    if (m_mount.contains("/boot")) {
        m_postcommand.append(QString("--use-boot yes --target-boot %1 --target-boot-fs %2 ")
                             .arg(trimDevice(m_mount["/boot"])).arg(m_mount["/boot"]->fileSystem().name()));
    } else {
        m_postcommand.append(QString("--use-boot no --target-boot %1 ").arg(trimDevice(m_mount["/"])));
    }

    if (m_mount.contains("/home")) {
        m_postcommand.append("--use-home yes ");
    } else {
        m_postcommand.append("--use-home no ");
    }

    if (m_mount.contains("/opt")) {
        m_postcommand.append("--use-opt yes ");
    } else {
        m_postcommand.append("--use-opt no ");
    }

    if (m_mount.contains("/tmp")) {
        m_postcommand.append("--use-tmp yes ");
    } else {
        m_postcommand.append("--use-tmp no ");
    }

    if (m_mount.contains("/usr")) {
        m_postcommand.append("--use-usr yes ");
    } else {
        m_postcommand.append("--use-usr no ");
    }

    if (m_mount.contains("/var")) {
        m_postcommand.append("--use-var yes ");
    } else {
        m_postcommand.append("--use-var no ");
    }

    if (!m_kblayout.isEmpty()) {
        m_postcommand.append(QString("--kblayout %1 ").arg(m_kblayout));
    }

    if (!m_kbvariant.isEmpty()) {
        m_postcommand.append(QString("--kbvariant %1 ").arg(m_kbvariant));
    }

    if (!m_hostname.isEmpty()) {
        m_postcommand.append(QString("--hostname %1 ").arg(m_hostname));
    }

    if (!m_timezone.isEmpty()) {
        m_postcommand.append(QString("--timezone %1 ").arg(m_timezone.replace('/', '-')));
    }

    if (!m_locale.isEmpty()) {
        m_postcommand.append(QString("--locale %1 ").arg(m_locale));
    }

    if (!m_KDELangPack.isEmpty() && m_KDELangPack != "en_us") {
        m_postcommand.append(QString("--kdelang %1 ").arg(m_KDELangPack));
    }

    if (m_configurePacman) {
        m_postcommand.append("--configure-pacman yes ");
    }

    if (m_installationType == NetInst) {
        m_postcommand.append("--netinst yes ");
    }
}

QString InstallationHandler::trimDevice(const Partition *device)
{
    QString rdv(device->devicePath());

    return QString("%1%2").arg(rdv.remove(0, 5)).arg(device->number());
}

void InstallationHandler::postRemove()
{
    qDebug() << " :: postinstall package removal: " <<
                m_removeLicenses.count() << "packages to be removed";

    if (m_stringlistIterator == m_removeLicenses.constEnd()) {
        emit streamLabel(i18n("Please wait, configuring the new system..."));

        QTimer::singleShot(100, this, SLOT(readyPost()));

        currAction = InstallationHandler::PostInstall;

        return;
    }

    QString command  = QString("sh " + QString(SCRIPTS_INSTALL_PATH) + "/postremove.sh --job %1 --mountpoint %2")
                       .arg(*m_stringlistIterator)
                       .arg(INSTALLATION_TARGET);

    m_process = new QProcess(this);

    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(postRemove()));

    ++m_stringlistIterator;

    m_process->start(command);

    qDebug() << " :: package removal command: " << command;
}

void InstallationHandler::postInstall()
{
    if (m_postjob == "add-extra-mountpoint") {
        qDebug() << " :: mountpoints to configure: " << m_mount.keys().count();
        QMap<QString, const Partition*>::const_iterator i;
        for (i = m_mount.constBegin(); i != m_mount.constEnd(); ++i) {
            if (i.key() != "/" && i.key() != "swap") {
                qDebug() << " :: add mountpoint for: " << i.key();
                QString command = QString("sh " + QString(SCRIPTS_INSTALL_PATH) +
                                          "/postinstall.sh --job add-extra-mountpoint --extra-mountpoint %1 "
                                          "--extra-mountpoint-target %2 %3")
                                          .arg(i.key())
                                          .arg(trimDevice(i.value()))
                                          .arg(m_postcommand);

                QProcess *process = new QProcess(this);
                QEventLoop e;
                connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), &e, SLOT(quit()));
                process->start(command);
                e.exec();

                qDebug() << " :: postinstall command: " << command;

                if (process->exitCode() != 0) {
                    postInstallDone(process->exitCode(), QProcess::NormalExit);
                    return;
                }
                process->deleteLater();
            }
        }

        postInstallDone(0, QProcess::NormalExit);
    } else {
        QString command  = QString("sh " + QString(SCRIPTS_INSTALL_PATH) +
                                   "/postinstall.sh --job %1 %2")
                                   .arg(m_postjob)
                                   .arg(m_postcommand);

        m_process = new QProcess(this);
        connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(postInstallDone(int, QProcess::ExitStatus)));
        m_process->start(command);

        qDebug() << " :: postinstall command: " << command;
    }
}

void InstallationHandler::postInstallDone(int eC, QProcess::ExitStatus eS)
{
    Q_UNUSED(eS)

    qDebug() << m_process->readAllStandardOutput();
    qDebug() << m_process->readAllStandardError();

    if (eC != 0) {
        emit errorInstalling(i18n("Error in Postinstall script. See log for more details."));
        qDebug() << " :: !! Failed during postinstall somewhere...";
        return;
    } else {
        // next job

        int percentage = 0;

        if (m_postjob == "initialize-target") {
            emit streamLabel(i18n("Creating user accounts..."));
            setUpUsers(userLoginList());
            m_postjob = "configure-pacman";
            m_postlabel = i18n("Configuring software management...");
            percentage = 1;
        } else if (m_postjob == "configure-pacman") {
            m_postjob = "pre-remove";
            m_postlabel = i18n("Removing unused packages...");
            percentage = 2;
        } else if (m_postjob == "pre-remove") {
            m_postjob = "pre-install";
            m_postlabel = i18n("Installing additional packages...");
            percentage = 3;
        } else if (m_postjob == "pre-install") {
            m_postjob = "setup-xorg";
            m_postlabel = i18n("Configuring the display...");
            percentage = 4;
        } else if (m_postjob == "setup-xorg") {
            m_postjob = "download-l10n";
            m_postlabel = i18n("Downloading and installing localization packages...");
            percentage = 5;
        } else if (m_postjob == "download-l10n") {
            m_postjob = "create-fstab";
            m_postlabel = i18n("Creating fstab...");
            percentage = 6;
        } else if (m_postjob == "create-fstab") {
            m_postjob = "add-extra-mountpoint";
            m_postlabel = i18n("Adding extra mountpoints...");
            percentage = 7;
        } else if (m_postjob == "add-extra-mountpoint") {
            m_postjob = "setup-hardware";
            m_postlabel = i18n("Configuring hardware...");
            percentage = 8;
        } else if (m_postjob == "setup-hardware") {
            m_postjob = "create-initrd";
            m_postlabel = i18n("Creating initial ramdisk images...");
            percentage = 9;
        } else if (m_postjob == "create-initrd") {
            m_postjob = "regenerate-locales";
            m_postlabel = i18n("Generating locales...");
            percentage = 10;
        } else if (m_postjob == "regenerate-locales") {
            m_postjob = "create-localtime";
            m_postlabel = i18n("Removing unused localizations...");
            percentage = 11;
        } else if (m_postjob == "create-localtime") {
            m_postjob = "set-keymap";
            m_postlabel = i18n("Symlinking localtime...");
            percentage = 12;
        } else if (m_postjob == "set-keymap") {
            m_postjob = "set-hostname";
            m_postlabel = i18n("Setting keymap...");
            percentage = 13;
        } else if (m_postjob == "set-hostname") {
            m_postjob = "cleanup-l10n";
            m_postlabel = i18n("Setting hostname...");
            percentage = 14;
        } else if (m_postjob == "cleanup-l10n") {
            m_postjob = "cleanup-drivers";
            m_postlabel = i18n("Removing unused drivers...");
            percentage = 15;
        } else if (m_postjob == "cleanup-drivers") {
            m_postjob = "cleanup-etc";
            m_postlabel = i18n("Finishing up...");
            percentage = 16;
        } else if (m_postjob == "cleanup-etc") {
            m_postjob = "jobcomplete";
            m_postlabel = i18n("Installation complete!");
            percentage = 17;
        }


        m_process->deleteLater();

        handleProgress(InstallationHandler::PostInstall, percentage);
        emit streamLabel(m_postlabel);

        if (m_postjob != "jobcomplete") {
            postInstall();

            return;
        } else {
            emit installationDone();
        }
    }
}

void InstallationHandler::addPartitionToMountList(const Partition *device, const QString &mountpoint)
{
    m_mount[mountpoint] = device;
}

void InstallationHandler::clearMounts()
{
    m_mount.clear();
}

QString InstallationHandler::getMountPointFor(const QString &device)
{
    QMap<QString, const Partition*>::const_iterator i;
    for (i = m_mount.constBegin(); i != m_mount.constEnd(); ++i) {
        if (QString("%1%2").arg(i.value()->devicePath()).arg(i.value()->number()) == device) {
            return i.key();
        }
    }

    return QString();
}

bool InstallationHandler::isMounted(const QString &partition)
{
    QMap<QString, const Partition*>::const_iterator i;
    for (i = m_mount.constBegin(); i != m_mount.constEnd(); ++i) {
        if (QString("%1%2").arg(i.value()->devicePath()).arg(i.value()->number()) == partition) {
            return i.value()->isMounted();
        }
    }

    return false;
}

QStringList InstallationHandler::getMountedPartitions()
{
    QStringList retlist;

    QMap<QString, const Partition*>::const_iterator i;
    for (i = m_mount.constBegin(); i != m_mount.constEnd(); ++i) {
        if (i.value()->isMounted()) {
            retlist.append(QString("%1%2").arg(i.value()->devicePath()).arg(i.value()->number()));
        }
    }

    return retlist;
}

void InstallationHandler::unmountPartition(const QString &partition)
{
    KIO::Job *job = KIO::unmount(partition, KIO::HideProgressInfo);
    KIO::NetAccess::synchronousRun(job, 0);
}

void InstallationHandler::mountNextPartition()
{
    if (m_mapIterator == m_mount.constEnd()) {
        copyFiles();
        return;
    }

    if (m_mapIterator.key() == "swap") {
        ++m_mapIterator;
        mountNextPartition();
        return;
    }

    QDir dir(QString(INSTALLATION_TARGET + m_mapIterator.key()));

    if (!dir.exists()) {
        if (!dir.mkpath(QString(INSTALLATION_TARGET + m_mapIterator.key()))) {
            emit errorInstalling(i18n("Tribe was not able to initialize needed directories. Something is very wrong..."));
            return;
        }
    }

    QString partitionname = QString("%1%2").arg(m_mapIterator.value()->devicePath()).arg(m_mapIterator.value()->number());

    qDebug() << " :: attempt to mount " << partitionname << " - " << m_mapIterator.key();

    emit mounting(partitionname, InstallationHandler::InProgress);

    KIO::SimpleJob *mJob = KIO::mount(false,
                                      QByteArray(),
                                      partitionname,
                                      QString(INSTALLATION_TARGET + m_mapIterator.key()),
                                      KIO::HideProgressInfo);

    connect(mJob, SIGNAL(result(KJob*)), SLOT(partitionMounted(KJob*)));
}

void InstallationHandler::partitionMounted(KJob *job)
{
    QString partitionname = QString("%1%2").arg(m_mapIterator.value()->devicePath()).arg(m_mapIterator.value()->number());
    if (job->error()) {
        emit errorInstalling(i18n("Could not mount %1", partitionname));
        qDebug() << " :: !! Error mounting: " << job->errorString();

        emit mounting(partitionname, InstallationHandler::Error);

        return;
    } else {
        emit mounting(partitionname, InstallationHandler::Success);

        ++m_mapIterator;

        mountNextPartition();
    }
}

void InstallationHandler::installBootloader(int action, const QString &device)
{
    Q_UNUSED(action);
    Q_UNUSED(device);

    if (m_process)
        m_process->deleteLater();

    QString command = QString("sh " + QString(SCRIPTS_INSTALL_PATH) +
                              "/postinstall.sh --job install-grub2 " + m_postcommand);

    m_process = new QProcess(this);
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SIGNAL(bootloaderInstalled(int, QProcess::ExitStatus)));

    qDebug() << " :: running bootloader install command: " << command;

    m_process->start(command);
}

void InstallationHandler::setUpUsers(QStringList users)
{
qDebug() << ":: Running setUpUsers() \n" << users << "\n\n";
    QString command;

    int current = 0;

    auto userNames= userNameList();
    auto userAvatars = userAvatarList();
    auto userLogins = userLoginList();
    auto userAutoLogins = userAutoLoginList();
    foreach(QString user, users) {
        if (checkExistingHomeDirs().contains(user)) {
            if (userNames.at(current) == "") {
               command = QString("chroot %1 useradd -d /home/%2 -s /bin/bash %2")
               .arg(INSTALLATION_TARGET)
               .arg(user);
            } else {
               command = QString("chroot %1 useradd -c \"%2\" -d /home/%3 -s /bin/bash %3")
               .arg(INSTALLATION_TARGET)
               .arg(userNames.at(current))
               .arg(user);
            }
qDebug() << " :: running useradd command: " << command;
            QProcess::execute(command);
        } else {
            if (userNames.at(current) == "") {
               command = QString("chroot %1 useradd -g users -m -s /bin/bash %2")
               .arg(INSTALLATION_TARGET)
               .arg(user);
            } else {
               command = QString("chroot %1 useradd -g users -c \"%2\" -m -s /bin/bash %3")
               .arg(INSTALLATION_TARGET)
               .arg(userNames.at(current))
               .arg(user);
            }
qDebug() << " :: running useradd command: " << command;
            QProcess::execute(command);
        }

qDebug() << " :: user \'" + user + "\' created";

        // set kdm/user avatar
        command = QString("bash -c \"mkdir -p " +
                          QString(INSTALLATION_TARGET) +
                          "/usr/share/apps/kdm/faces > /dev/null 2>&1\"");
        QProcess::execute(command);
        command = QString("bash -c \"cp " + userAvatars.at(current) + " " +
                          QString(INSTALLATION_TARGET) + "/usr/share/apps/kdm/faces/" +
                          user + ".face.icon > /dev/null 2>&1\"");
        QProcess::execute(command);
        command = QString("bash -c \"cp " + userAvatars.at(current) + " " +
                          QString(INSTALLATION_TARGET) + "/home/" +
                          user + "/.face.icon > /dev/null 2>&1\"");
        QProcess::execute(command);

        // set autologin
        if (userAutoLogins.at(current) == "1") {
            command = QString("bash -c \"sed -i -e \'s/#AutoLoginEnable=true/AutoLoginEnable=true/\' " +
                              QString(INSTALLATION_TARGET) + "/usr/share/config/kdm/kdmrc\"");
            QProcess::execute(command);
            command = QString("bash -c \"sed -i -e \'s/#AutoLoginUser=fred/AutoLoginUser=" + user + "/\' " +
                              QString(INSTALLATION_TARGET) + "/usr/share/config/kdm/kdmrc\"");
            QProcess::execute(command);
        }

qDebug() << " :: setting user password... : " << userLogins.at(current);
        // set user passwd
        m_userProcess = new QProcess(this);
        m_passwdCount = current;
        command = QString("chroot %1 /usr/bin/passwd %2")
                          .arg(INSTALLATION_TARGET)
                          .arg(user);
        connect(m_userProcess, SIGNAL(readyReadStandardError()), SLOT(streamPassword()));
        m_userProcess->start(command);
        sleep(3);
        m_userProcess->waitForFinished();

qDebug() << " :: live configuration copied to the user's home directory";

        QProcess::execute("sh " +
                          QString(SCRIPTS_INSTALL_PATH) +
                          "/postinstall.sh --job configure-users " +
                          m_postcommand +
                          " --user-name " +
                          user);

qDebug() << " :: user configuration complete";

        QProcess::execute("sh " +
                          QString(SCRIPTS_INSTALL_PATH) +
                          "/postinstall.sh --job configure-sudoers " +
                          m_postcommand +
                          " --user-name " +
                          user);

qDebug() << " :: sudoers configuration complete";

        current++;
    }

qDebug() << " :: setting root password...";

    // set root passwd
    m_rootUserProcess = new QProcess(this);
    m_passwdCount = current;
    command = QString("chroot %1 /usr/bin/passwd").arg(INSTALLATION_TARGET);
    connect(m_rootUserProcess, SIGNAL(readyReadStandardError()), SLOT(streamRootPassword()));
    m_rootUserProcess->start(command);
    sleep(3);
    m_rootUserProcess->waitForFinished();
}

void InstallationHandler::streamPassword()
{
    m_userProcess->write(QString(userPasswordList().at(m_passwdCount)).toUtf8().data());
    m_userProcess->write("\n");

    sleep(3);
}

void InstallationHandler::streamRootPassword()
{
    m_rootUserProcess->write(m_rootPassword.toUtf8().data());
    m_rootUserProcess->write("\n");

    sleep(3);
}

void InstallationHandler::unmountAll()
{
    for (QMap<QString, const Partition*>::const_iterator i = m_mount.constBegin(); i != m_mount.constEnd(); ++i) {
        if (i.key() != "/") {
            QProcess::execute("bash -c \"sudo umount -fl " + QString(INSTALLATION_TARGET + i.key()) + " > /dev/null 2>&1\"");
        }
    }

    foreach (const QString &point, QStringList() << "/proc" << "/sys" << "/dev")
    {
        QProcess::execute("bash -c \"sudo umount -fl " + QString(INSTALLATION_TARGET + point) + " > /dev/null 2>&1\"");
    }

    QProcess::execute("bash -c \"sudo umount -fl " + QString(INSTALLATION_TARGET) + " > /dev/null 2>&1\"");
}

void InstallationHandler::abortInstallation()
{
    cleanup();
}

void InstallationHandler::killProcesses()
{
    if (m_process) {
        m_process->terminate();
        m_process->waitForFinished(100);
        if (m_process->state() != QProcess::NotRunning) {
            m_process->kill();
        }
        m_process->waitForFinished(100);
    }
}

void InstallationHandler::cleanup()
{
    if (QFile::exists("/tmp/installation.log")) {
        qDebug() << " :: copying installation logs to target /var/log";
        QFile::copy("/tmp/installation.log", QString(INSTALLATION_TARGET) + "/var/log/installation.log");
    }

    if (QFile::exists("/tmp/initramfs.log")) {
        qDebug() << " :: copying initramfs logs to target /var/log";
        QFile::copy("/tmp/initramfs.log", QString(INSTALLATION_TARGET) + "/var/log/installation-initramdisk.log");
    }

    if (QFile::exists("/tmp/grub2.log")) {
        qDebug() << " :: copying grub2 logs to target /var/log";
        QFile::copy("/tmp/grub2.log", QString(INSTALLATION_TARGET) + "/var/log/installation-grub2.log");
    }

    killProcesses();
    unmountAll();
}

#include "installationhandler.moc"
