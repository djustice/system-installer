#ifndef CONFIGUREBOOTLOADER_H
#define CONFIGUREBOOTLOADER_H

#include <QDebug>

#include <QProcess>
#include <QTimer>


class ConfigureBootloader : public QProcess
{
    Q_OBJECT

public:

    ConfigureBootloader(QObject *parent);
    ~ConfigureBootloader();

    void install();

    bool open(QIODeviceBase::OpenMode mode) override;
    bool waitForReadyRead(int msecs) override;
    bool waitForBytesWritten(int msecs) override;
    qint64 bytesToWrite() const override;
    void close() override;

    QString m_bootDevice;
    QString m_rootDevice;

public slots:

    void failed();

};

#endif // MOUNTROOT_H
