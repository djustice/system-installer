#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// QtCore
#include <QFile>
#include <QProcess>
#include <QTextStream>
#include <QTimeZone>
#include <QString>
#include <QStringList>

// QtGui
#include <QListWidget>
#include <QMainWindow>
#include <QPalette>
#include <QSpacerItem>

// QtNetwork
#include <QTcpSocket>

// local
#include "colorchooser.h"
#include "colorizedpixmap.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void previousButtonClicked();
    void continueButtonClicked();
    void quitButtonClicked();

    void preparePartitionsButtonClicked();
    void deviceComboTextChanged(QString text);

    void timeZoneClicked();
    void langPackClicked();
    void networkCheckClicked();
    void colorButtonClicked();

    void validateNetworkPage();
    void validateUserPage();
    void validateSoftwarePage();

private:
    Ui::MainWindow *ui;
    ColorChooser *colorChooser;
    ColorizedPixmap *colorizedLogoPixmap;

    int m_currentPage;

    QProcess *p;

    QStringList m_timeZones;
    QStringList m_langPacks;

    QString m_lsblkOutput;
};
#endif // MAINWINDOW_H
