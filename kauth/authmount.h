#ifndef AUTHMOUNT_H
#define AUTHMOUNT_H

#include <QDebug>
#include <QFile>
#include <QObject>
#include <QProcess>
#include <QStringLiteral>

#include <KAuth/ActionReply>
#include <KAuth/HelperSupport>

class InstallationHandler;

using namespace KAuth;

class AuthMount : public QObject
{
    Q_OBJECT

public Q_SLOTS:
    ActionReply mount(const QVariantMap &args);
    ActionReply unmount(const QVariantMap &args);
    void parseUnsquashFsOutput();
    ActionReply unsquash(const QVariantMap &args);
    void unsquashFsDone(int);
    ActionReply bootctl();
    // void handleProgress(int);

Q_SIGNALS:
    void percentage(int);

private:
    InstallationHandler *m_installationHandler;
    QProcess *m_process;

};

#endif // AUTHMOUNT_H
