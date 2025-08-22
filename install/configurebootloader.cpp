#include "configurebootloader.h"

ConfigureBootloader::ConfigureBootloader(QObject *parent)
{
    this->setParent(parent);
}

ConfigureBootloader::~ConfigureBootloader()
{

}

void ConfigureBootloader::install()
{
    QProcessEnvironment env;
    env.insert(QString::fromUtf8("LC_ALL"), QString::fromUtf8("C"));
    setProcessEnvironment(env);
    setProcessChannelMode(QProcess::MergedChannels);

    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(failed()));

    qDebug() << " :: configure bootloader";

    start("sudo", QStringList() << "/usr/bin/arch-chroot" <<
                                   "/new/root" <<
                                   "grub-install" <<
                                   "--target=x86_64-efi" <<
                                   "--efi-directory=/boot" <<
                                   "--bootloader-id=GRUB");

    waitForFinished(-1);

    start("sudo", QStringList() << "/usr/bin/arch-chroot" <<
                                   "/new/root"
                                   "grub-mkconfig" <<
                                   "-o" <<
                                   "/boot/grub/grub.cfg");

    waitForFinished(-1);

    readAll().clear();

    start("sudo", QStringList() << "/usr/bin/bash" << "-c" << "lsblk -lf | grep " + m_bootDevice.last(4) + " | xargs | cut -d' ' -f4");

    waitForFinished(-1);

    QString bootDevUUID = readAll();

    readAll().clear();

    start("sudo", QStringList() << "/usr/bin/bash" << "-c" << "lsblk -lf | grep " + m_rootDevice.last(4) + " | xargs | cut -d' ' -f4");

    waitForFinished(-1);

    QString rootDevUUID = readAll();

    readAll().clear();

    qDebug() << " :: bootloader stop";
}

void ConfigureBootloader::failed()
{
    qDebug() << " !! configure bootloader failed: " << "\n" << readAll();
}

bool ConfigureBootloader::open(QIODeviceBase::OpenMode mode)
{
    return true;
}

bool ConfigureBootloader::waitForReadyRead(int msecs)
{
    return true;
}

bool ConfigureBootloader::waitForBytesWritten(int msecs)
{
    return true;
}

qint64 ConfigureBootloader::bytesToWrite() const
{
    return 0;
}

void ConfigureBootloader::close()
{

}
