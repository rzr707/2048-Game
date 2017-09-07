#ifndef RECORDLABEL_H
#define RECORDLABEL_H

#include <QLabel>

class RecordLabel : public QLabel {
public:
    RecordLabel(QString text, QLabel* parent = 0)
        : QLabel(parent) {
        setText(text);
        setAttribute(Qt::WA_Hover, true);
        setMouseTracking(true);
    }

    void changeColor(QColor color) {
        QPalette pal;
        pal.setColor(QPalette::Text, Qt::red);
        setPalette(pal);
    }

protected:
    void mousePressEvent() {
        qDebug() << "clicked";
    }

    void enterEvent() {
        changeColor(Qt::red);
        qDebug() << "RecordLabel enterEvent called";
    }

    void leaveEvent() {
        changeColor(Qt::black);
    }

};

#endif // RECORDLABEL_H
