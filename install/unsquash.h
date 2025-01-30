#ifndef UNSQUASH_H
#define UNSQUASH_H

#include <QDebug>

#include <QProcess>
#include <QTimer>


class UnsquashRoot : public QProcess
{
    Q_OBJECT

public:

    UnsquashRoot(QObject *parent);
    ~UnsquashRoot();

    void Unsquash();

    bool open(QIODeviceBase::OpenMode mode) override;
    bool waitForReadyRead(int msecs) override;
    bool waitForBytesWritten(int msecs) override;
    qint64 bytesToWrite() const override;
    void close() override;

    QString m_rootDevice;

public slots:

    void printProgress();
    void failedUnsquash();

};

#endif // UNSQUASH_H
