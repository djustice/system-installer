#ifndef CONFIGUREBOOTLOADER_H
#define CONFIGUREBOOTLOADER_H

#include <QObject>
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QProcessEnvironment>
#include <QStringList>

class ConfigureBootloader : public QProcess
{
    Q_OBJECT

public:
    explicit ConfigureBootloader(QObject *parent = nullptr);
    ~ConfigureBootloader();

    void setBootDevice(const QString &device);
    void setRootDevice(const QString &device);
    void install();

    // QIODevice interface overrides
    bool open(QIODeviceBase::OpenMode mode) override;
    void close() override;
    bool waitForReadyRead(int msecs) override;
    bool waitForBytesWritten(int msecs) override;
    qint64 bytesToWrite() const override;

protected:
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;

private slots:
    void failed();

private:
    QString m_bootDevice;
    QString m_rootDevice;
    QStringList m_grubEntry;
};

#endif // CONFIGUREBOOTLOADER_H
