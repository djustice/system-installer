#ifndef MOUNTBOOT_H
#define MOUNTBOOT_H

#include <QDebug>

#include <QProcess>
#include <QTimer>


class MountBoot : public QProcess
{
    Q_OBJECT

public:

    MountBoot(QObject *parent);
    ~MountBoot();

    void MkDir();
    void Mount();

    bool open(QIODeviceBase::OpenMode mode) override;
    bool waitForReadyRead(int msecs) override;
    bool waitForBytesWritten(int msecs) override;
    qint64 bytesToWrite() const override;
    void close() override;

    QString m_rootDevice;

public slots:

    void failedMkdir();
    void failedMount();

// protected:
//
//     qint64 readData(char* data, qint64 maxlen) override;
//     qint64 writeData(const char* data, qint64 len) override;

};

#endif // MOUNTROOT_H
