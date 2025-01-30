#include "unsquash.h"


UnsquashRoot::UnsquashRoot(QObject *parent)
{
      this->setParent(parent);
}

UnsquashRoot::~UnsquashRoot()
{

}

void UnsquashRoot::Unsquash()
{
      QProcessEnvironment env;
      env.insert(QString::fromUtf8("LC_ALL"), QString::fromUtf8("C"));
      setProcessEnvironment(env);
      setProcessChannelMode(QProcess::MergedChannels);

      connect(this, SIGNAL(readyReadStandardError()), this, SLOT(failedUnsquash()));
      connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(printProgress()));

      //
      // UNSQUASH TO ROOT

      qDebug() << "unsquash root start";

      start(QString("sudo"),
            QStringList() << "/usr/bin/unsquashfs" <<
            QString("-d") <<
            QString("/new/root") <<
            QString("-f") <<
            QString("/root/x86_64/system.sfs"));

      if (!waitForStarted()) {
            qDebug() << "Error starting process:" << errorString();
      }

      waitForFinished(-1);

      qDebug() << "unsquash done: " << "\n" << readAll();
}

void UnsquashRoot::printProgress()
{
      qDebug() << "unsquash: " << readAll();
}


void UnsquashRoot::failedUnsquash()
{
      qDebug() << "unsquash failed: " << "\n" << readAll();
}

bool UnsquashRoot::open(QIODeviceBase::OpenMode mode)
{
      return true;
}

bool UnsquashRoot::waitForReadyRead(int msecs)
{
      return true;
}

bool UnsquashRoot::waitForBytesWritten(int msecs)
{
      return true;
}

qint64 UnsquashRoot::bytesToWrite() const
{
      return 0;
}

void UnsquashRoot::close()
{

}
