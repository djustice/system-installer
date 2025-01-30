#include "mountroot.h"
#include <qobjectdefs.h>

MountRoot::MountRoot(QObject *parent)
{
      this->setParent(parent);
}

MountRoot::~MountRoot()
{

}

void MountRoot::MkDir()
{
    QProcessEnvironment env;
    env.insert(QString::fromUtf8("LC_ALL"), QString::fromUtf8("C"));
    setProcessEnvironment(env);
    setProcessChannelMode(QProcess::MergedChannels);

    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(failedMkdir()));

    //
    // MKDIRS

    qDebug() << "mkdir -p /new/root";

    start(QString("sudo"),
          QStringList() << QString("/usr/bin/mkdir") <<
          QString("-p") <<
          QString("/new/root"));

    waitForFinished(-1);

    qDebug() << "mkdir done: " << "\n" << readAll();
}

void MountRoot::failedMkdir()
{
    qDebug() << "mkdir failed: " << "\n" << readAll();
}

void MountRoot::Mount()
{
    disconnect(this, SIGNAL(readyReadStandardError()), this, SLOT(failedMkdir()));

    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(failedMount()));

    //
    // MOUNT ROOT

    qDebug() << "mount new root" << m_rootDevice << " to /new/root";

    waitForFinished();

    start(QString("sudo"),
          QStringList() << "/usr/bin/mount" <<
          m_rootDevice <<
          QString("/new/root"));

    waitForFinished(-1);

    qDebug() << "mounted";
}

void MountRoot::failedMount()
{
    qDebug() << "mount failed: " << "\n" << readAll();
}

bool MountRoot::open(QIODeviceBase::OpenMode mode)
{
      return true;
}

bool MountRoot::waitForReadyRead(int msecs)
{
      return true;
}

bool MountRoot::waitForBytesWritten(int msecs)
{
      return true;
}

qint64 MountRoot::bytesToWrite() const
{
      return 0;
}

void MountRoot::close()
{

}
