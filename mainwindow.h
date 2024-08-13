#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// KDE
#include <KF6/KCoreAddons/KAboutData>
#include <KF6/KI18n/KLocalizedString>

// QtCore
#include <QAction>
#include <QDir>
#include <QFile>
#include <QFileInfoList>
#include <QListIterator>
#include <QProcess>
#include <QTextStream>
#include <QThread>
#include <QTimeZone>
#include <QString>
#include <QStringList>

// QtGui
#include <QIcon>
#include <QListWidget>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QPalette>
#include <QSpacerItem>
#include <qt6/QtWidgets/QWidget>

// QtNetwork
#include <QTcpSocket>
#include <QTimer>

// local
#include "colorchooser.h"
#include "colorizedpixmap.h"
#include "installationhandler.h"

// autouic
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void appendColorProcessOutput(QString output);

public Q_SLOTS:
    void previousButtonClicked();
    void continueButtonClicked();
    void quitButtonClicked();

    void preparePartitionsButtonClicked();
    void preparePartitionsProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void deviceComboTextChanged(QString text);
    double convertHumanSizeToBytes(QString text);
    void destBootComboTextChanged(QString text);
    void destRootComboTextChanged(QString text);
    void destSwapComboTextChanged(QString text);

    void timeZoneClicked();
    void langPackClicked();
    void keymapClicked();
    void networkCheckClicked();
    void colorButtonClicked();

    void generateIconTheme();

    void validateNetworkPage();
    void validateUserPage();
    void validateSoftwarePage();

    void updateUi();

private:
    Ui::MainWindow *ui;
    ColorChooser *colorChooser;
    ColorizedPixmap *colorizedLogoPixmap;
    InstallationHandler *installationHandler;

    void validatePartitionPage();

    int m_currentPage;

    QMenu *localeMenu;
    QList<QAction> localeActions;

    QProcess *p;
    QProcess *m_iconColorProcess;

    QThread *m_colorProcessThread;
    QTimer *m_iconColorTimer;

    QStringList m_timeZones;
    QStringList m_langPacks;

    QString m_lsblkOutput;

    bool currentRootPartValid = false;
    bool currentBootPartValid = false;
    bool currentSwapPartValid = false;
};
#endif // MAINWINDOW_H
