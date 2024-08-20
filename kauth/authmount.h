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
    ActionReply unsquash(const QVariantMap &args);
    ActionReply configure(const QVariantMap &args);
    ActionReply bootctl();
    ActionReply unmount(const QVariantMap &args);
    ActionReply movetousr(const QVariantMap &args);

    // void handleProgress(int);
    void unsquashFsDone(int); // ?
    void parseUnsquashFsOutput(); // ?

Q_SIGNALS:
    void percentage(int);

private:
    InstallationHandler *m_installationHandler;
    QProcess *m_process;

};

#endif // AUTHMOUNT_H
