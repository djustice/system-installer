#ifndef COLORCHOOSER_H
#define COLORCHOOSER_H

#include <QCheckBox>
#include <QLabel>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QScreen>
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

    QPushButton *colorButton;
    ColorizedPixmap *logoLabel;
    QWidget *installerWidget;
    QPushButton *colorPushButton;
    QRadioButton *colorRadioButton;
    QCheckBox *colorCheckBox;

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    Ui::ColorChooser *ui;
    QScreen *primaryScreen;
    int screenWidth;
    int mouseX;
    QColor mouseColor;
};

#endif // COLORCHOOSER_H
