
#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QThread>

#include "kauth/authmount.h"

ActionReply AuthMount::mount(const QVariantMap &args)
{
    QString root = args["root"].toString();
    QString boot = args["boot"].toString();

    QProcess pMountRoot;
    pMountRoot.start("mount", QStringList() << root << "/new/root");
    pMountRoot.waitForFinished();
    pMountRoot.start("mkdir", QStringList() << "/new/root/boot");
    pMountRoot.waitForFinished();
    QProcess pMountBoot;
    pMountBoot.start("mount", QStringList() << boot << "/new/root/boot");
    pMountBoot.waitForFinished();

    m_process = new QProcess(this);
    QProcessEnvironment env;
    env.insert("LC_ALL", "C");
    m_process->setProcessEnvironment(env);
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    return ActionReply::SuccessReply();
}

ActionReply AuthMount::unmount(const QVariantMap& args)
{
    QString root = args["root"].toString();
    QString boot = args["boot"].toString();

    m_process->start("sync", QStringList());
    m_process->waitForFinished();
    m_process->start("umount", QStringList() << root << "/new/root/boot");
    m_process->waitForFinished();
    m_process->start("umount", QStringList() << boot << "/new/root");
    m_process->waitForFinished();

    return ActionReply::SuccessReply();
}

ActionReply AuthMount::unsquash(const QVariantMap& args)
{
    QString sfs = args["sfs"].toString();

    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(unsquashFsDone(int)));
    connect(m_process, SIGNAL(readyRead()), this, SLOT(parseUnsquashFsOutput()));
    m_process->start("unsquashfs", QStringList() << "-f" << "-d" << "/new/root" << sfs, QIODevice::ReadOnly);

    return ActionReply::SuccessReply();
}

void AuthMount::parseUnsquashFsOutput()
{
    QString out = m_process->readLine(2048);

    if (out.contains(QChar(']'))) {     // todo: make me pretty
        QString parsed = out.split(QChar(']')).at(1);
        QStringList members = parsed.split(QChar('/'));
        QString firstMem = members.at(0);
        firstMem = firstMem.remove(QChar(' '));
        int one = firstMem.toInt();
        QString secondMem = members.at(1);
        secondMem = secondMem.split(QChar(' ')).at(0);
        int two = secondMem.toInt();
        int val = (one * 100) / two;
        qDebug() << val;
        HelperSupport::progressStep(val);
        QFile f("/tmp/unsqfs");
        f.remove();
        f.open(QIODevice::WriteOnly);
        QTextStream(&f) << val;
    }
}

// void AuthMount::handleProgress(int i)
// {
//     qDebug() << i;
// }

ActionReply AuthMount::bootctl()
{
    QProcess p;
    p.execute("arch-chroot", QStringList() << "/new/root" << "bootctl" << "install");
    p.waitForFinished();

    return ActionReply::SuccessReply();
}


void AuthMount::unsquashFsDone(int i)
{
    qDebug() << "unsquash complete: " << i;
}

KAUTH_HELPER_MAIN("org.kde.systeminstaller", AuthMount)
