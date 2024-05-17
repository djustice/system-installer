#include "installationhandler.h"

#include <QStyleFactory>
#include <QThread>

#include <KAuth/Action>
#include <KAuth/ExecuteJob>

using namespace KAuth;

InstallationHandler::InstallationHandler()
{

}

void InstallationHandler::init(QWidget* parent)
{
    this->setParent(parent);
    m_process = new QProcess(this);
    QProcessEnvironment env;
    env.insert(QString::fromUtf8("LC_ALL"), QString::fromUtf8("C"));
    m_process->setProcessEnvironment(env);
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    QProcess p;
    p.start(QString::fromUtf8("mkdir"), QStringList() << QString::fromUtf8("/new/root"));
    p.waitForFinished();
    p.start(QString::fromUtf8("mkdir"), QStringList() << QString::fromUtf8("/new/root/boot"));
    p.waitForFinished();
    p.start(QString::fromUtf8("mkdir"), QStringList() << QString::fromUtf8("/new/swap"));
    p.waitForFinished();

    QVariantMap args;
    args[QString::fromUtf8("root")] = m_rootDevice;
    args[QString::fromUtf8("boot")] = m_bootDevice;
    args[QString::fromUtf8("sfs")] = QString::fromUtf8("/root/x86_64/system.sfs");

    Action mountRootAction(QString::fromUtf8("org.kde.systeminstaller.mount"));
    mountRootAction.setHelperId(QString::fromUtf8("org.kde.systeminstaller"));
    mountRootAction.setArguments(args);
    ExecuteJob *mountRootJob = mountRootAction.execute();
    qDebug() << "org.kde.systeminstaller.mount: start";
    mountRootJob->exec();
    qDebug() << "org.kde.systeminstaller.mount: end";

    Action unsquashAction(QString::fromUtf8("org.kde.systeminstaller.unsquash"));
    unsquashAction.setHelperId(QString::fromUtf8("org.kde.systeminstaller"));
    unsquashAction.setArguments(args);
    unsquashAction.setTimeout(800000);
    ExecuteJob *unsquashJob = unsquashAction.execute();
    qDebug() << "org.kde.systeminstaller.unsquash: start";
    unsquashJob->exec();
    qDebug() << "org.kde.systeminstaller.unsquash: end";

    Action bootctlAction(QString::fromUtf8("org.kde.systeminstaller.bootctl"));
    bootctlAction.setHelperId(QString::fromUtf8("org.kde.systeminstaller"));
    bootctlAction.setArguments(args);
    ExecuteJob *bootctlJob = bootctlAction.execute();
    qDebug() << "org.kde.systeminstaller.bootctl: start";
    bootctlJob->exec();
    qDebug() << "org.kde.systeminstaller.bootctl: end";

    Action unmountRootAction(QString::fromUtf8("org.kde.systeminstaller.unmount"));
    unmountRootAction.setHelperId(QString::fromUtf8("org.kde.systeminstaller"));
    unmountRootAction.setArguments(args);
    ExecuteJob *unmountRootJob = unmountRootAction.execute();
    qDebug() << "org.kde.systeminstaller.unmount: start";
    unmountRootJob->exec();
    qDebug() << "org.kde.systeminstaller.unmount: end";
}

void InstallationHandler::percent(KJob* job, unsigned long val)
{
    qDebug() << "perc: " << val;
}

void InstallationHandler::unsquashResult(KJob *kjob)
{
    auto job = qobject_cast<KAuth::ExecuteJob *>(kjob);
    qDebug() << "unsq res: " << job->error() << job->data();
}

void InstallationHandler::installSystem()
{
    // sqfs located at /root/x86_64/system.sfs
    // connect(m_process, SIGNAL(readyRead()), SLOT(parseUnsquashfsOutput()));
    // connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(unsquashDone(int)));
    //
    // QStringList arguments;
    // arguments << "-f" << "-d" << "/new/root" << QString("/root/x86_64/system.sfs");
    //
    // m_process->start("unsquashfs", arguments);

    /////////
    // QVariantMap unsquashArgs;
    // unsquashArgs["device"] = m_bootDevice;
    // unsquashArgs["path"] = "/new/root/boot";
    //
    // Action unsquashAction("org.kde.systeminstaller.unsquash");
    // unsquashAction.setHelperId("org.kde.systeminstaller");
    // unsquashAction.setArguments(unsquashArgs);
    // ExecuteJob *unsquashJob = unsquashAction.execute();
    // if (!unsquashJob->exec()) {
    //     qDebug() << "KAuth returned an error:" << unsquashJob->errorText();
    // } else {
    //     QString contents = unsquashJob->data()["contents"].toString();
    //     qDebug() << "KAuth succeeded. Contents: " << contents;
    // }
}

void InstallationHandler::parseUnsquashfsOutput(int i)
{
    qDebug() << i;
}

void InstallationHandler::handleProgress(const QVariantMap &data)
{
    qDebug() << data;
}

void InstallationHandler::unsquashDone(int i)
{
    qDebug() << "unsquashDone: exitcode: " << i;
}

void InstallationHandler::partitionMounted(KJob* job)
{
    if (job->error() == 0) {
        KIO::SimpleJob *mBootJob = KIO::mount(false,
                                QByteArray(),
                                m_bootDevice,
                                QString::fromUtf8("/new/root/boot"),
                                KIO::HideProgressInfo);
        mBootJob->start();
    }
}

void InstallationHandler::unmountPartition(QString partition) {
    KIO::Job *job = KIO::unmount(partition, KIO::HideProgressInfo);
    connect(job, SIGNAL(result(KJob*)), SLOT(partitionUnmounted(KJob*)));
}

void InstallationHandler::partitionUnmounted(KJob* job)
{
    qDebug() << "partition unmounted";
}

InstallationHandler::~InstallationHandler()
{

}
