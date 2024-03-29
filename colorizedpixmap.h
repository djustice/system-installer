#ifndef COLORIZEDPIXMAP_H
#define COLORIZEDPIXMAP_H

#include <QBrush>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QPixmap>
#include <QWidget>

class ColorizedPixmap : public QWidget
{
    Q_OBJECT

public:
    explicit ColorizedPixmap(QWidget *parent = nullptr);

    QColor color;

protected:
    void paintEvent(QPaintEvent *) override;

};

#endif // COLORIZEDPIXMAP_H
