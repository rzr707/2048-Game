/**
  void moveCell(Move m) - двигает ячейку (анимация, используется)
*/
#ifndef CELL
#define CELL

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QPainter>
#include <QFont>
#include <QPropertyAnimation> // for animated tiles

#include <QRadialGradient>
#include <QDebug>

#include <QThread>
#include <QApplication>

enum Move { LEFT, RIGHT, DOWN, UP };

class Cell : public QWidget {
    Q_OBJECT
private:
    QLabel*             m_plbl;
    uint                m_nNumber;
    bool                m_bIsEmpty;  // показатель, если на циферблате ноль
    QPropertyAnimation* m_pan;


protected:
    virtual void paintEvent(QPaintEvent*) {
        QBrush brush(Qt::red, Qt::SolidPattern);

        // color changer
        switch(getNumber()) {
        case 2:
            brush.setColor(Qt::white);
            break;
        case 4:
            brush.setColor(QColor("#F29498"));
            break;
        case 8:
            brush.setColor(QColor("#B2F294"));
            break;
        case 16:
            brush.setColor(QColor("#D0DE3A"));
            break;
        case 32:
            brush.setColor(QColor("#94F2A7"));
            break;
        case 64:
            brush.setColor(QColor("#94F2EE"));
            break;
        case 128:
            brush.setColor(QColor("#949CF2"));
            break;
        case 256:
            brush.setColor(QColor("#BE94F2"));
            break;
        case 512:
            brush.setColor(QColor("#FF1212"));
            break;
        case 1024:
            brush.setColor(QColor("#FF9500"));
            break;
        case 2048:
            brush.setColor(QColor("#E205FF"));
            break;
        default:
            brush.setColor(Qt::lightGray);
            break;
        }

        QPen     pen;
        pen.setWidth(20);
        QPainter p;
        p.begin(this);
            p.setPen(pen);
            p.setBrush(brush);
            p.drawRect(0, 0, width(), height());
        p.end();
    }

    /* Для изменения размера шрифта при изменении размера окна */
    virtual void resizeEvent(QResizeEvent*) {
        QFont font = this->font();
        if(getNumber() < 1000)
            font.setPointSize(width() / 8);
        else
            font.setPointSize(width() / 10);
        setFont(font);
    }


    /*
     * Конструктор должен создавать ячейку с нулём, а уже в desk.h в начале рандомно появляются
     * две ненулевые ячейки с 2 или 4
    */
public:
    Cell(QWidget* pwgt = 0) : QWidget(pwgt), m_bIsEmpty(true) {
        m_nNumber = (rand() % 2);
        m_plbl = new QLabel("<font size = 25>" + QString::number(m_nNumber) + "</font>");
        m_pan = new QPropertyAnimation(this, "geometry");
        QVBoxLayout* pvbl = new QVBoxLayout;
        pvbl->addWidget(m_plbl, 0, Qt::AlignCenter | Qt::AlignVCenter);
        setLayout(pvbl);
        resize(35, 35);
    }

    uint getNumber(void) { return m_nNumber; }

    void setNumber(uint n) {
        m_nNumber = n;
        if(!n) {
            m_plbl->clear();
            m_bIsEmpty = true;
        } else {
            m_plbl->setText("<font size = 25>" + QString::number(m_nNumber) + "</font>");
        }
    }

    void moveCell(Move m) {
        m_pan->setDuration(100);
        m_pan->setEasingCurve(QEasingCurve::SineCurve); // Animation

        switch(m) {
        case LEFT:
            m_pan->setEndValue(QRect(this->x() - width(),
                                     this->y(),
                                     width(),
                                     height()
                                     )
                               );
            break;
        case RIGHT:
            m_pan->setEndValue(QRect(x() + width(),
                                     y(),
                                     width(),
                                     height()
                                     )
                               );
            break;
        case DOWN:
            m_pan->setEndValue(QRect(x(),
                                     y() + height(),
                                     width(),
                                     height()
                                     )
                               );
            break;
        case UP:
            m_pan->setEndValue(QRect(x(),
                                     y() - height(),
                                     width(),
                                     height()
                                     )
                               );
            break;
        }
        m_pan->start();
    }

};

#endif // CELL

