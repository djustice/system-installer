#include "mountroot.h"

MountRoot::MountRoot()
{

}

MountRoot::~MountRoot()
{

}

void MountRoot::MountRootInit()
{
    QProcessEnvironment env;
    env.insert(QString::fromUtf8("LC_ALL"), QString::fromUtf8("C"));
    setProcessEnvironment(env);
    setProcessChannelMode(QProcess::MergedChannels);

    //
    // MKDIRS

    qDebug() << "mkdir -p /new/root";

    start(QString("sudo"),
          QStringList() << QString("/usr/bin/mkdir") <<
          QString("-p") <<
          QString("/new/root"));

    waitForFinished();

    qDebug() << "mkdir done: " << "\n" << readAll();

    //
    // MOUNT ROOT

    qDebug() << "mount new root" << m_rootDevice << " to /new/root";

    waitForFinished();
    start(QString("sudo"),
          QStringList() << "/usr/bin/mount" <<
          m_rootDevice <<
          QString("/new/root"));
    waitForFinished();

    qDebug() << "mounted";
}
