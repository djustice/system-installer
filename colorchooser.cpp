#include "colorchooser.h"
#include "ui_colorchooser.h"

ColorChooser::ColorChooser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorChooser)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_Hover);
    setMouseTracking(true);
    grabMouse();
    primaryScreen = QApplication::primaryScreen();
    screenWidth = primaryScreen->geometry().width();
    mouseColor = Qt::white;
}

void ColorChooser::mouseMoveEvent(QMouseEvent *event)
{
    mouseX = QCursor::pos().x();

    if (mouseX < 100)
        mouseX = 0;

    if (mouseX > screenWidth - 100)
        mouseX = screenWidth;

    int hue = (mouseX * 255) / screenWidth;
    QColor hsv = Qt::white;
    hsv.setHsv(hue, 255, 255);

    mouseColor = hsv;

    repaint();
}

void ColorChooser::mousePressEvent(QMouseEvent *event)
{
    releaseMouse();
    colorButton->setStyleSheet("QPushButton#colorButton { background-color : " + mouseColor.name() + "; color : " + mouseColor.name() + "; }");
    logoLabel->color = mouseColor;
    logoLabel->repaint();
    installerWidget->setStyleSheet("QPushButton { background-color: black; \
                                                  padding: 4px; \
                                                  color : " + mouseColor.name() + "; \
                                                  border: 1px solid " + mouseColor.name() + "; } \
                                    QLabel { color : " + mouseColor.name() + "; } \
                                    QRadioButton { color : " + mouseColor.name() + "; } \
                                    QCheckBox { color : " + mouseColor.name() + "; } \
                                    QComboBox { color : " + mouseColor.name() + "; \
                                                background-color : #000000; } \
                                    QGroupBox { color : " + mouseColor.name() + "; } \
                                    QLineEdit { color : " + mouseColor.name() + "; } \
                                    QListWidget { color : " + mouseColor.name() + "; \
                                                  selection-color : #000000; \
                                                  selection-background-color : " + mouseColor.name() + "; } \
                                    QScrollBar::vertical { border : 1px solid " + mouseColor.name() + "; \
                                                           background : #000000; \
                                                           width : 15px; } \
                                    QScrollBar::handle { background : #000000; border : 1px solid " + mouseColor.name() + "; } \
                                    QScrollBar::up-arrow::vertical, QScrollBar::down-arrow::vertical { background : #000000; border : 1px solid " + mouseColor.name() + "; } \
                                    QScrollBar::add-line::vertical, QScrollBar::sub-line::vertical { background : #000000; border : 1px solid " + mouseColor.name() + "; } \
                                    QDateTimeEdit { color : " + mouseColor.name() + "; }");
    this->close();
}

void ColorChooser::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), QPixmap(":images/images/wallpaper_system.png"));
    painter.setCompositionMode(QPainter::CompositionMode_Darken);
    QBrush brush(mouseColor);
    QPen pen(brush, 3000);
    painter.setPen(pen);
    painter.drawLine(0, 0, 3000, 3000);
}

ColorChooser::~ColorChooser()
{
    delete ui;
}
