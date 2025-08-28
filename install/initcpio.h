#ifndef INITCPIO_H
#define INITCPIO_H

#include <QDebug>

#include <QProcess>
#include <QTimer>


class InitCpio : public QProcess
{
    Q_OBJECT

public:

    InitCpio(QObject *parent);
    ~InitCpio();

    void MkInitCpio();

    bool open(QIODeviceBase::OpenMode mode) override;
    bool waitForReadyRead(int msecs) override;
    bool waitForBytesWritten(int msecs) override;
    qint64 bytesToWrite() const override;
    void close() override;

    QString m_rootDevice;
    QString m_bootDevice;

public slots:

    void failedMkInitCpio();

signals:

    void updateProgress(QString progress);

};

#endif // INITCPIO_H
