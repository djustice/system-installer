#include "installationhandler.h"

#include <QStyleFactory>
#include <QThread>

InstallationHandler::InstallationHandler()
{

}

void InstallationHandler::init(QWidget* parent)
{
    this->setParent(parent);
    m_process = new QProcess(this);
    QProcessEnvironment env;
    env.insert(QString::fromUtf8("LC_ALL"), QString::fromUtf8("C"));
    m_process->setProcessEnvironment(env);
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    //
    // MKDIRS

    qDebug() << "mkdir -p /new/root";

    m_process->start(QString("sudo"),
                     QStringList() << QString("/usr/bin/mkdir") <<
                     QString("-p") <<
                     QString("/new/root"));

    m_process->waitForFinished();

    qDebug() << "mkdir done";

    //
    // MOUNT ROOT

    qDebug() << "mount new root" << m_rootDevice << " to /new/root";

    m_process->waitForFinished();
    m_process->start(QString("sudo"),
                     QStringList() << "/usr/bin/mount" <<
                     m_rootDevice <<
                     QString("/new/root"));
    m_process->waitForFinished();

    qDebug() << "mounted";

    //
    // UNSQUASH TO ROOT

    qDebug() << "unsquash root start";

    m_process->start(QString("sudo"),
                     QStringList() << "/usr/bin/unsquashfs" <<
                     QString("-d") <<
                     QString("/new/root") <<
                     QString("-f") <<
                     QString("/root/x86_64/system.sfs"));

    if (!m_process->waitForStarted()) {
        qDebug() << "Error starting process:" << m_process->errorString();
    }

    m_process->waitForFinished();
    qDebug() << m_process->readAllStandardOutput();
    qDebug() << m_process->readAllStandardError();

    m_process->waitForFinished();

    qDebug() << "unsquash root end";

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

    // TODO: test if 'exit' chroot required here.

    m_process->start("sudo",
                     QStringList() << "/usr/bin/arch-chroot" <<
                     "/new/root"
                     "grub-mkconfig" <<
                     "-o" <<
                     "/boot/grub/grub.cfg");

    m_process->waitForFinished();

    qDebug() << "bootloader stop";

    //
    // UNMOUNT FILESYSTEMS

    qDebug() << "org.kde.systeminstaller.unmount: start";

    qDebug() << "org.kde.systeminstaller.unmount: end";
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

InstallationHandler::~InstallationHandler()
{

}
