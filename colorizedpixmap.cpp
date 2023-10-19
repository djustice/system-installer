#include "colorizedpixmap.h"

ColorizedPixmap::ColorizedPixmap(QWidget *parent)
    : QWidget{parent}
{
    color = QColor(255, 255, 255, 255);
}

void ColorizedPixmap::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), QPixmap(":/images/images/logo.png"));
    painter.setCompositionMode(QPainter::CompositionMode_Darken);
    painter.fillRect(0, 0, 666, 100, color);
}
