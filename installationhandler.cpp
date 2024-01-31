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
    env.insert("LC_ALL", "C");
    m_process->setProcessEnvironment(env);
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    // qDebug() << "installationhandler::init()";
    //
    // KIO::MkdirJob *mMkdirRootJob = KIO::mkdir(QUrl("/new/root"));
    // mMkdirRootJob->start();
    //
    //     qDebug() << "installationhandler::init() 2";
    // KIO::MkdirJob *mMkdirBootJob = KIO::mkdir(QUrl("/new/boot"));
    // mMkdirBootJob->start();
    // KIO::MkdirJob *mMkdirSwapJob = KIO::mkdir(QUrl("/new/swap"));
    // mMkdirSwapJob->start();

    QProcess p;
    p.start("mkdir", QStringList() << "/new/root");
    p.waitForFinished();
    p.start("mkdir", QStringList() << "/new/root/boot");
    p.waitForFinished();
    p.start("mkdir", QStringList() << "/new/swap");
    p.waitForFinished();

    QVariantMap args;
    args["root"] = m_rootDevice;
    args["boot"] = m_bootDevice;
    args["sfs"] = "/root/x86_64/system.sfs";

    Action mountRootAction("org.kde.systeminstaller.mount");
    mountRootAction.setHelperId("org.kde.systeminstaller");
    mountRootAction.setArguments(args);
    ExecuteJob *mountRootJob = mountRootAction.execute();
    mountRootJob->exec();

    Action unsquashAction("org.kde.systeminstaller.unsquash");
    unsquashAction.setHelperId("org.kde.systeminstaller");
    unsquashAction.setArguments(args);
    unsquashAction.setParentWidget(parent);
    unsquashAction.setTimeout(800000);
    ExecuteJob *unsquashJob = unsquashAction.execute();
    unsquashJob->exec();

    Action bootctlAction("org.kde.systeminstaller.bootctl");
    bootctlAction.setHelperId("org.kde.systeminstaller");
    bootctlAction.setArguments(args);
    ExecuteJob *bootctlJob = bootctlAction.execute();
    bootctlJob->exec();

    Action unmountRootAction("org.kde.systeminstaller.unmount");
    unmountRootAction.setHelperId("org.kde.systeminstaller");
    unmountRootAction.setArguments(args);
    ExecuteJob *unmountRootJob = unmountRootAction.execute();
    unmountRootJob->exec();
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

    ///////////
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
                                "/new/root/boot",
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
