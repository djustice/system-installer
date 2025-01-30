#ifndef INSTALLATIONHANDLER_H
#define INSTALLATIONHANDLER_H

#include <QDebug>
#include <QWidget>
#include <QObject>
#include <QTime>
#include <QProcess>
#include <QMap>
#include <QPointer>

#include <KF6/KIOCore/KIO/CopyJob>
#include <KF6/KIOCore/KIO/SimpleJob>
#include <KF6/KIOCore/KIO/MkdirJob>

#include <install/mountroot.h>
#include <install/unsquash.h>

struct UserAccountData {
    QString login;
    QString password;
    QString name;
    QString autoLogin;
};

class InstallationHandler : public QObject
{
    Q_OBJECT

public:
    explicit InstallationHandler();
    ~InstallationHandler();

    void init(QWidget*);
    void unmountPartition(QString partition);
    void installSystem();

    void copyFiles();

    void installBootloader(int, const QString&);

    void setUpUsers(QStringList);

    void setHostname(const QString &name) { m_hostname = name; }
    QString hostname() { return m_hostname; }

    void setContinent(const QString &continent) { m_continent = continent; }
    QString continent() { return m_continent; }

    void setRegion(const QString &region) { m_region = region; }
    QString region() { return m_region; }

    void setKbLayout(const QString &kblayout) { m_kblayout = kblayout; }
    QString kblayout() { return m_kblayout; }

    void setKbVariant(const QString &kbvariant) { m_kbvariant = kbvariant; }
    QString kbvariant() { return m_kbvariant; }

    void setTimezone(const QString &time) { m_timezone = time; }
    QString timezone() { return m_timezone; }

    void setLocale(const QString &loc) { m_locale = loc; }
    QString locale() { return m_locale; }

    void setKDELangPack(const QString &pack) { m_KDELangPack = pack; }
    QString KDELangPack() { return m_KDELangPack; }

    void setPackages(const QStringList & packages) { m_packages = packages; }
    QStringList packages() const { return m_packages; }

    void setRootPassword(QString password) { m_rootPassword = password; }
    QString rootPassword() {return m_rootPassword; }

    void setRootDevice(const QString& s) { m_rootDevice = s; }
    QString rootDevice() { return m_rootDevice; }

    void setBootDevice(const QString& s) { m_bootDevice = s; }
    QString bootDevice() { return m_bootDevice; }

public Q_SLOTS:
    void parseUnsquashfsOutput(int);
    void unsquashDone(int);
    void handleProgress(const QVariantMap&);

    void initFinished();
    void mountFinished();
    void unsquashFinished();
    void mkinitcpioFinished();
    void installBootloaderFinished();
    void unmountFinished();

private:
    QPointer<QProcess> m_process;
    QProcess *m_initProcess;
    QProcess *m_mountProcess;
    QProcess *m_unsquashProcess;
    QProcess *m_mkinitcpioProcess;
    QProcess *m_installBootloader;
    QProcess *m_unmountProcess;

    QProcess *m_userProcess;
    QProcess *m_rootUserProcess;

    QStringList m_mtab;

    UserAccountData m_userAccountData;
    QString m_rootPassword;

    QStringList m_packages;

    QString m_kblayout;
    QString m_kbvariant;

    QString m_hostname;
    QString m_continent;
    QString m_region;
    QString m_timezone;
    QString m_locale;
    QString m_KDELangPack;
    QString m_rootDevice;
    QString m_bootDevice;
};

#endif // INSTALLATIONHANDLER_H
