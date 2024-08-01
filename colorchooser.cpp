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
    m_primaryScreen = QApplication::primaryScreen();
    m_screenWidth = m_primaryScreen->geometry().width();
    m_mouseColor = Qt::white;
}

void ColorChooser::mouseMoveEvent(QMouseEvent *event)
{
    m_mouseX = QCursor::pos().x();

    if (m_mouseX < 100)
        m_mouseX = 0;

    if (m_mouseX > m_screenWidth - 100)
        m_mouseX = m_screenWidth;

    int hue = (m_mouseX * 255) / m_screenWidth;
    QColor hsv = Qt::white;
    hsv.setHsv(hue, 255, 255);

    m_mouseColor = hsv;

    repaint();
}

void ColorChooser::mousePressEvent(QMouseEvent *event)
{
    releaseMouse();
    this->setVisible(false);
    m_colorButton->setStyleSheet(QStringLiteral("QPushButton#colorButton { background-color : ") + m_mouseColor.name() + QStringLiteral("; color : ") + m_mouseColor.name() + QStringLiteral("; }"));
    m_logoLabel->color = m_mouseColor;
    m_logoLabel->repaint();
    m_installerWidget->setStyleSheet(QStringLiteral( \
                                   "QPushButton { background-color: black; \
                                                  padding: 4px; \
                                                  color : ") + m_mouseColor.name() + QStringLiteral("; \
                                                  border: 1px solid ") + m_mouseColor.name() + QStringLiteral("; } \
                                    QLabel { color : ") + m_mouseColor.name() + QStringLiteral("; } \
                                    QRadioButton { color : ") + m_mouseColor.name() + QStringLiteral("; } \
                                    QCheckBox { color : ") + m_mouseColor.name() + QStringLiteral("; } \
                                    QComboBox { color : ") + m_mouseColor.name() + QStringLiteral("; \
                                                background-color : #000000; } \
                                    QGroupBox { color : ") + m_mouseColor.name() + QStringLiteral("; } \
                                    QLineEdit { color : ") + m_mouseColor.name() + QStringLiteral("; } \
                                    QListWidget { color : ") + m_mouseColor.name() + QStringLiteral("; \
                                                  selection-color : #000000; \
                                                  selection-background-color : ") + m_mouseColor.name() + QStringLiteral("; } \
                                    QScrollBar::vertical { border : 1px solid ") + m_mouseColor.name() + QStringLiteral("; \
                                                           background : #000000; \
                                                           width : 15px; } \
                                    QScrollBar::handle { background : #000000; border : 1px solid ") + m_mouseColor.name() + QStringLiteral("; } \
                                    QScrollBar::up-arrow::vertical, QScrollBar::down-arrow::vertical { background : #000000; border : 1px solid ") + m_mouseColor.name() + QStringLiteral("; } \
                                    QScrollBar::add-line::vertical, QScrollBar::sub-line::vertical { background : #000000; border : 1px solid ") + m_mouseColor.name() + QStringLiteral("; } \
                                    QDateTimeEdit { color : ") + m_mouseColor.name() + QStringLiteral("; }"));

    emit done();
    this->close();
}

void ColorChooser::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), QPixmap(QString::fromUtf8(":images/images/wallpaper_system.png")));
    painter.setCompositionMode(QPainter::CompositionMode_Darken);
    QBrush brush(m_mouseColor);
    QPen pen(brush, 3000);
    painter.setPen(pen);
    painter.drawLine(0, 0, 3000, 3000);
}

ColorChooser::~ColorChooser()
{
    delete ui;
}
