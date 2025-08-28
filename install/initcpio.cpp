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

      start(QString("sudo"), QStringList() << "/usr/bin/cp"
                                           << "/etc/pacman.d/mirrorlist"
                                           << "/new/root/etc/pacman.d/mirrorlist");
      waitForFinished(-1);

      start(QString("sudo"), QStringList() << "/usr/bin/arch-chroot"
                                           << "/new/root"
                                           << "pacman-key"
                                           << "--init");
      waitForFinished(-1);

      start(QString("sudo"), QStringList() << "/usr/bin/arch-chroot"
                                           << "/new/root"
                                           << "/usr/bin/bash"
                                           << "-c"
                                           << "sed -e's/Required DatabaseOptional/TrustAll/' -i /etc/pacman.conf");
      waitForFinished(-1);

      start(QString("sudo"), QStringList() << "/usr/bin/arch-chroot"
                                           << "/new/root"
                                           << "pacman"
                                           << "-Sy"
                                           << "--noconfirm"
                                           << "linux");

      if (!waitForStarted()) {
            qDebug() << " !! error starting process:" << errorString();
      }

      waitForFinished(-1);

      qDebug() << " :: mkinitcpio done: " << "\n" << readAll();
}

void InitCpio::failedMkInitCpio()
{
      qDebug() << " :: mkinitcpio failed: " << "\n" << readAll();
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
