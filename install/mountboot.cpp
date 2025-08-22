#include "mountboot.h"
#include <qobjectdefs.h>

MountBoot::MountBoot(QObject *parent)
{
      this->setParent(parent);
}

MountBoot::~MountBoot()
{

}

void MountBoot::MkDir()
{
      QProcessEnvironment env;
      env.insert(QString::fromUtf8("LC_ALL"), QString::fromUtf8("C"));
      setProcessEnvironment(env);
      setProcessChannelMode(QProcess::MergedChannels);

      connect(this, SIGNAL(readyReadStandardError()), this, SLOT(failedMkdir()));

    //
    // MKDIRS

      qDebug() << "mkdir -p /new/boot";

      start(QString("sudo"),
            QStringList() << QString("/usr/bin/mkdir") <<
            QString("-p") <<
            QString("/new/root/boot"));

      waitForFinished(-1);

      qDebug() << "mkdir done: " << "\n" << readAll();
}

void MountBoot::failedMkdir()
{
      qDebug() << "mkdir failed: " << "\n" << readAll();
}

void MountBoot::Mount()
{
      disconnect(this, SIGNAL(readyReadStandardError()), this, SLOT(failedMkdir()));

      connect(this, SIGNAL(readyReadStandardError()), this, SLOT(failedMount()));

      //
      // MOUNT BOOT

      qDebug() << "mount new boot" << m_rootDevice << " to /new/root/boot";

      waitForFinished();

      start(QString("sudo"),
            QStringList() << "/usr/bin/mount" <<
            m_rootDevice <<
            QString("/new/root/boot"));

      waitForFinished(-1);

      qDebug() << "mounted";
}

void MountBoot::failedMount()
{
      qDebug() << "mount failed: " << "\n" << readAll();
}

bool MountBoot::open(QIODeviceBase::OpenMode mode)
{
      return true;
}

bool MountBoot::waitForReadyRead(int msecs)
{
      return true;
}

bool MountBoot::waitForBytesWritten(int msecs)
{
      return true;
}

qint64 MountBoot::bytesToWrite() const
{
      return 0;
}

void MountBoot::close()
{

}
