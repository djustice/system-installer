#include "initcpio.h"

InitCpio::InitCpio(QObject *parent)
{
    this->setParent(parent);
}

InitCpio::~InitCpio()
{

}

void InitCpio::MkInitCpio()
{
    QProcessEnvironment env;
    env.insert(QString::fromUtf8("LC_ALL"), QString::fromUtf8("C"));
    setProcessEnvironment(env);
    setProcessChannelMode(QProcess::MergedChannels);

    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(failedMkInitCpio()));

    qDebug() << " :: mkinitcpio start";

    // Copy mirrorlist
    start(QString("sudo"), QStringList() << "/usr/bin/cp"
                                         << "/etc/pacman.d/mirrorlist"
                                         << "/new/root/etc/pacman.d/mirrorlist");
    waitForFinished(-1);

    if (exitCode() != 0) {
        qDebug() << " !! failed to copy mirrorlist:" << readAll();
        return;
    }

    // Initialize pacman keyring
    qDebug() << " :: initializing pacman keyring";
    start(QString("sudo"), QStringList() << "/usr/bin/arch-chroot"
                                         << "/new/root"
                                         << "pacman-key"
                                         << "--init");
    waitForFinished(-1);

    if (exitCode() != 0) {
        qDebug() << " !! failed to initialize pacman-key:" << readAll();
        return;
    }

    // Populate archlinux keyring
    qDebug() << " :: populating archlinux keyring";
    start(QString("sudo"), QStringList() << "/usr/bin/arch-chroot"
                                         << "/new/root"
                                         << "pacman-key"
                                         << "--populate"
                                         << "archlinux");
    waitForFinished(-1);

    // Modify pacman.conf to trust all packages temporarily
    qDebug() << " :: modifying pacman.conf";
    start(QString("sudo"), QStringList() << "/usr/bin/arch-chroot"
                                         << "/new/root"
                                         << "/usr/bin/bash"
                                         << "-c"
                                         << "sed -e 's/Required DatabaseOptional/TrustAll/' -i /etc/pacman.conf");
    waitForFinished(-1);

    if (exitCode() != 0) {
        qDebug() << " !! failed to modify pacman.conf:" << readAll();
    }

    // Install linux kernel (which triggers mkinitcpio)
    qDebug() << " :: installing linux kernel";
    start(QString("sudo"), QStringList() << "/usr/bin/arch-chroot"
                                         << "/new/root"
                                         << "pacman"
                                         << "-Sy"
                                         << "--noconfirm"
                                         << "linux");

    if (!waitForStarted()) {
        qDebug() << " !! error starting process:" << errorString();
        return;
    }

    waitForFinished(-1);

    if (exitCode() == 0) {
        qDebug() << " :: mkinitcpio done successfully";
    } else {
        qDebug() << " !! mkinitcpio failed with exit code:" << exitCode();
        qDebug() << readAll();
        return;
    }

    // Restore pacman.conf to original state
    qDebug() << " :: restoring pacman.conf";
    start(QString("sudo"), QStringList() << "/usr/bin/arch-chroot"
                                         << "/new/root"
                                         << "/usr/bin/bash"
                                         << "-c"
                                         << "sed -e 's/TrustAll/Required DatabaseOptional/' -i /etc/pacman.conf");
    waitForFinished(-1);

    // Optionally verify mkinitcpio was generated
    start(QString("sudo"), QStringList() << "/usr/bin/bash"
                                         << "-c"
                                         << "ls -lh /new/root/boot/initramfs-linux*.img");
    waitForFinished(-1);

    qDebug() << " :: initramfs images:" << readAll();
}

void InitCpio::failedMkInitCpio()
{
    qDebug() << " !! mkinitcpio failed: " << "\n" << readAll();
}

bool InitCpio::open(QIODeviceBase::OpenMode mode)
{
    return true;
}

bool InitCpio::waitForReadyRead(int msecs)
{
    return true;
}

bool InitCpio::waitForBytesWritten(int msecs)
{
    return true;
}

qint64 InitCpio::bytesToWrite() const
{
    return 0;
}

void InitCpio::close()
{

}

qint64 InitCpio::readData(char *data, qint64 maxSize)
{
    return 0;
}

qint64 InitCpio::writeData(const char *data, qint64 maxSize)
{
    return maxSize;
}
