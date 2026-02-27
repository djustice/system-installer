#ifndef INITCPIO_H
#define INITCPIO_H

#include <QObject>
#include <QProcess>
#include <QDebug>
#include <QProcessEnvironment>

class InitCpio : public QProcess
{
    Q_OBJECT

public:
    explicit InitCpio(QObject *parent = nullptr);
    ~InitCpio();

    void MkInitCpio();

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
    void failedMkInitCpio();
};

#endif // INITCPIO_H
