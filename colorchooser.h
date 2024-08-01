#ifndef COLORCHOOSER_H
#define COLORCHOOSER_H

//QtCore
#include <QProcess>

//QtGui
#include <QCheckBox>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QScreen>
#include <QTimer>
#include <QWidget>

#include "colorizedpixmap.h"

namespace Ui {
class ColorChooser;
}

class ColorChooser : public QWidget
{
    Q_OBJECT

public:
    explicit ColorChooser(QWidget *parent = nullptr);
    ~ColorChooser();

    QCheckBox *m_colorCheckBox;
    QPushButton *m_colorButton;
    QPushButton *m_colorPushButton;
    QRadioButton *m_colorRadioButton;
    QWidget *m_installerWidget;

    QTimer *m_colorIconSetTimer;

    ColorizedPixmap *m_logoLabel;

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void colorChanged(QColor color);
    void done();

private:
    Ui::ColorChooser *ui;

    QColor m_mouseColor;
    QScreen *m_primaryScreen;
    QProcess *m_colorProcess;

    int m_screenWidth;
    int m_mouseX;
};

#endif // COLORCHOOSER_H
