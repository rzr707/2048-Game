#ifndef HEADERLABEL_H
#define HEADERLABEL_H

#include <QtWidgets>
#include "recordlabel.h"

class HeaderLabelWidget : public QWidget {
    Q_OBJECT
public:
    HeaderLabelWidget(QWidget* parent = 0)
        : QWidget(parent) {

        QFont font;
        font.setPointSize(36);
        RecordLabel* pName = new RecordLabel("NAME:");
        pName->setFont(font);
        RecordLabel* pRecord = new RecordLabel("SCORE:");
        pRecord->setFont(font);
        RecordLabel* pPos    = new RecordLabel("POSITION:");
        pPos->setFont(font);

        QHBoxLayout* pLayout = new QHBoxLayout(this);
        pLayout->addWidget(pPos, 0, Qt::AlignCenter);
        pLayout->addWidget(pName, 0, Qt::AlignCenter);
        pLayout->addWidget(pRecord, 0, Qt::AlignCenter);
        setLayout(pLayout);
    }
};

#endif // HEADERLABEL_H
