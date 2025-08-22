#include "mountroot.h"

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

    qDebug() << " :: mkdir -p /new/root";

    start("sudo", QStringList() << "/usr/bin/mkdir" <<
                                   "-p" <<
                                   "/new/root");

    waitForFinished(-1);

    qDebug() << " :: mkdir done: " << "\n" << readAll();
}

void MountRoot::failedMkdir()
{
    qDebug() << " !! mkdir failed: " << "\n" << readAll();
}

void MountRoot::Mount()
{
    disconnect(this, SIGNAL(readyReadStandardError()), this, SLOT(failedMkdir()));
    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(failedMount()));

    qDebug() << " :: mount new root" << m_rootDevice << " to /new/root";

    waitForFinished();

    start("sudo", QStringList() << "/usr/bin/mount" <<
                                     m_rootDevice <<
                                     "/new/root");

    waitForFinished(-1);

    qDebug() << " :: done";
}

void MountRoot::failedMount()
{
    qDebug() << " !! mount failed: " << "\n" << readAll();
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
