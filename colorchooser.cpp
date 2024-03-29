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
    colorButton->setStyleSheet(QStringLiteral("QPushButton#colorButton { background-color : ") + mouseColor.name() + QStringLiteral("; color : ") + mouseColor.name() + QStringLiteral("; }"));
    logoLabel->color = mouseColor;
    logoLabel->repaint();
    installerWidget->setStyleSheet(QStringLiteral( \
                                   "QPushButton { background-color: black; \
                                                  padding: 4px; \
                                                  color : ") + mouseColor.name() + QStringLiteral("; \
                                                  border: 1px solid ") + mouseColor.name() + QStringLiteral("; } \
                                    QLabel { color : ") + mouseColor.name() + QStringLiteral("; } \
                                    QRadioButton { color : ") + mouseColor.name() + QStringLiteral("; } \
                                    QCheckBox { color : ") + mouseColor.name() + QStringLiteral("; } \
                                    QComboBox { color : ") + mouseColor.name() + QStringLiteral("; \
                                                background-color : #000000; } \
                                    QGroupBox { color : ") + mouseColor.name() + QStringLiteral("; } \
                                    QLineEdit { color : ") + mouseColor.name() + QStringLiteral("; } \
                                    QListWidget { color : ") + mouseColor.name() + QStringLiteral("; \
                                                  selection-color : #000000; \
                                                  selection-background-color : ") + mouseColor.name() + QStringLiteral("; } \
                                    QScrollBar::vertical { border : 1px solid ") + mouseColor.name() + QStringLiteral("; \
                                                           background : #000000; \
                                                           width : 15px; } \
                                    QScrollBar::handle { background : #000000; border : 1px solid ") + mouseColor.name() + QStringLiteral("; } \
                                    QScrollBar::up-arrow::vertical, QScrollBar::down-arrow::vertical { background : #000000; border : 1px solid ") + mouseColor.name() + QStringLiteral("; } \
                                    QScrollBar::add-line::vertical, QScrollBar::sub-line::vertical { background : #000000; border : 1px solid ") + mouseColor.name() + QStringLiteral("; } \
                                    QDateTimeEdit { color : ") + mouseColor.name() + QStringLiteral("; }"));
    this->close();
}

void ColorChooser::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), QPixmap(QString::fromUtf8(":images/images/wallpaper_system.png")));
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
