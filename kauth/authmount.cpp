
#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QThread>

#include "kauth/authmount.h"

ActionReply AuthMount::mount(const QVariantMap &args)
{
    QString root = args[QStringLiteral("root")].toString();
    QString boot = args[QStringLiteral("boot")].toString();

    QProcess pMountRoot;
    pMountRoot.start(QStringLiteral("mount"), QStringList() << root << QStringLiteral("/new/root"));
    pMountRoot.waitForFinished();
    pMountRoot.start(QStringLiteral("mkdir"), QStringList() << QStringLiteral("/new/root/boot"));
    pMountRoot.waitForFinished();
    QProcess pMountBoot;
    pMountBoot.start(QStringLiteral("mount"), QStringList() << boot << QStringLiteral("/new/root/boot"));
    pMountBoot.waitForFinished();

    m_process = new QProcess(this);
    QProcessEnvironment env;
    env.insert(QStringLiteral("LC_ALL"), QStringLiteral("C"));
    m_process->setProcessEnvironment(env);
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    return ActionReply::SuccessReply();
}

ActionReply AuthMount::unmount(const QVariantMap& args)
{
    QString root = args[QStringLiteral("root")].toString();
    QString boot = args[QStringLiteral("boot")].toString();

    m_process->start(QStringLiteral("sync"), QStringList());
    m_process->waitForFinished();
    m_process->start(QStringLiteral("umount"), QStringList() << root << QStringLiteral("/new/root/boot"));
    m_process->waitForFinished();
    m_process->start(QStringLiteral("umount"), QStringList() << boot << QStringLiteral("/new/root"));
    m_process->waitForFinished();

    return ActionReply::SuccessReply();
}

ActionReply AuthMount::unsquash(const QVariantMap& args)
{
    QString sfs = args[QStringLiteral("sfs")].toString();

    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(unsquashFsDone(int)));
    connect(m_process, SIGNAL(readyRead()), this, SLOT(parseUnsquashFsOutput()));
    m_process->start(QStringLiteral("unsquashfs"), QStringList() << QStringLiteral("-f") << QStringLiteral("-d") << QStringLiteral("/new/root") << sfs, QIODevice::ReadOnly);

    return ActionReply::SuccessReply();
}

void AuthMount::parseUnsquashFsOutput()
{
    // QString out = m_process->readLine(2048);
    //
    // if (out.contains(QChar(']'))) {     // todo: make me pretty
    //     QString parsed = out.split(QChar(']')).at(1);
    //     QStringList members = parsed.split(QChar('/'));
    //     QString firstMem = members.at(0);
    //     firstMem = firstMem.remove(QChar(' '));
    //     int one = firstMem.toInt();
    //     QString secondMem = members.at(1);
    //     secondMem = secondMem.split(QChar(' ')).at(0);
    //     int two = secondMem.toInt();
    //     int val = (one * 100) / two;
    //     qDebug() << val;
    //     HelperSupport::progressStep(val);
    //     QFile f("/tmp/unsqfs");
    //     f.remove();
    //     f.open(QIODevice::WriteOnly);
    //     QTextStream(&f) << val;
    // }
}

// void AuthMount::handleProgress(int i)
// {
//     qDebug() << i;
// }

ActionReply AuthMount::bootctl()
{
    QProcess p;
    p.execute(QStringLiteral("arch-chroot"), QStringList() << QStringLiteral("/new/root") << QStringLiteral("bootctl") << QStringLiteral("install"));
    p.waitForFinished();

    return ActionReply::SuccessReply();
}


void AuthMount::unsquashFsDone(int i)
{
    qDebug() << "unsquash complete: " << i;
}

KAUTH_HELPER_MAIN("org.kde.systeminstaller", AuthMount)
