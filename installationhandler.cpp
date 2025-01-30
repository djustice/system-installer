#include "installationhandler.h"

#include <QStyleFactory>
#include <QThread>
#include <QTimer>

InstallationHandler::InstallationHandler()
{

}

void InstallationHandler::init(QWidget* parent)
{
    this->setParent(parent);
    m_initProcess = new QProcess(this);
    QProcessEnvironment env;
    env.insert(QString::fromUtf8("LC_ALL"), QString::fromUtf8("C"));
    m_initProcess->setProcessEnvironment(env);
    m_initProcess->setProcessChannelMode(QProcess::MergedChannels);

    MountRoot *mountRoot = new MountRoot(this);
    mountRoot->m_rootDevice = m_rootDevice;
    mountRoot->MkDir();
    mountRoot->Mount();

    UnsquashRoot *unsquashRoot = new UnsquashRoot(this);
    unsquashRoot->Unsquash();

/*
    //
    // INSTALL BOOTLOADER

    qDebug() << "bootloader start";

    m_process->start(QString("sudo"),
                     QStringList() << "/usr/bin/mount" <<
                     m_bootDevice <<
                     QString("/new/root/boot"));

    m_process->waitForFinished();

    m_process->start(QString("sudo"),
                     QStringList() << "/usr/bin/arch-chroot" <<
                     QString("/new/root") <<
                     QString("grub-install") <<
                     QString("--target=x86_64-efi") <<
                     QString("--efi-directory=").append(QString("/boot")) <<
                     QString("--bootloader-id=GRUB"));

    m_process->waitForFinished();

    m_process->start("sudo",
                     QStringList() << "/usr/bin/arch-chroot" <<
                     "/new/root"
                     "grub-mkconfig" <<
                     "-o" <<
                     "/boot/grub/grub.cfg");

    m_process->waitForFinished();

    qDebug() << "bootloader stop";

    //
    // INITCPIO FILESYSTEMS

    qDebug() << "initcpio start";

    m_process->start("sudo",
                     QStringList() << "/usr/bin/arch-chroot" <<
                     "/new/root"
                     "mkinitcpio" <<
                     "-P");

    m_process->waitForFinished();

    qDebug() << "initcpio end";

    //
    // UNMOUNT FILESYSTEMS

    qDebug() << "org.kde.systeminstaller.unmount: start";

    m_process->start("sudo",
                     QStringList() << "/usr/bin/umount" <<
                     "/new/root/boot");
    m_process->waitForFinished();
    m_process->start("sudo",
                     QStringList() << "/usr/bin/umount" <<
                     "/new/root");
    m_process->waitForFinished();


    qDebug() << "org.kde.systeminstaller.unmount: end";*/
}

void InstallationHandler::installSystem()
{
    // // sqfs located at /root/x86_64/system.sfs
    //
    // connect(m_process, SIGNAL(readyRead()), SLOT(parseUnsquashfsOutput()));
    // connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(unsquashDone(int)));
    //
    // QStringList arguments;
    // arguments << "-f" << "-d" << "/new/root" << QString("/root/x86_64/system.sfs");
    //
    // m_process->start("unsquashfs", arguments);
    //
    // ///////
    // QVariantMap unsquashArgs;
    // unsquashArgs["device"] = m_bootDevice;
    // unsquashArgs["path"] = "/new/root/boot";
    //
    // Action unsquashAction("org.kde.systeminstaller.unsquash");
    // unsquashAction.setHelperId("org.kde.systeminstaller");
    // unsquashAction.setArguments(unsquashArgs);
    // ExecuteJob *unsquashJob = unsquashAction.execute();
    // if (!unsquashJob->exec()) {
    //     qDebug() << "KAuth returned an error:" << unsquashJob->errorText();
    // } else {
    //     QString contents = unsquashJob->data()["contents"].toString();
    //     qDebug() << "KAuth succeeded. Contents: " << contents;
    // }
}

void InstallationHandler::parseUnsquashfsOutput(int i)
{
    qDebug() << i;
}

void InstallationHandler::handleProgress(const QVariantMap &data)
{
    qDebug() << data;
}

void InstallationHandler::unsquashDone(int i)
{
    qDebug() << "unsquashDone: exitcode: " << i;
}

void InstallationHandler::initFinished()
{

}

void InstallationHandler::mountFinished()
{

}

void InstallationHandler::copyFiles()
{

}

void InstallationHandler::unsquashFinished()
{

}

void InstallationHandler::installBootloaderFinished()
{

}

void InstallationHandler::mkinitcpioFinished()
{

}

void InstallationHandler::setUpUsers(QStringList)
{

}

void InstallationHandler::unmountFinished()
{

}



InstallationHandler::~InstallationHandler()
{

}
