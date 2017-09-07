#ifndef SCOREWINDOW_H
#define SCOREWINDOW_H

#include <QtWidgets>
#include <QtXml>
#include <stdexcept>
#include "recordlabel.h"
#include "headerlabel.h"

class ScoreWindow : public QWidget {
    Q_OBJECT

private:
    QScrollArea*            pscrollArea;
    QVBoxLayout*            pScoreLayout;
    QString                 xmlFilePath;
    QFile                   xmlFile;
    QFont                   font;
    QMultiMap<int, QString> records;

public:
    ScoreWindow(QFile& xml,
                QString player,
                QString record,
                QWidget* parent = nullptr)
        : QWidget(parent)
        , pscrollArea(new QScrollArea(this)) {

        QWidget* proxyWidget = new QWidget(this);
        xmlFilePath = xml.fileName();
        addRecord(player, record);

        font.setPointSize(15);
        QDomDocument doc;

        try {
            if(!xml.open(QIODevice::ReadOnly)) {
                throw std::runtime_error(std::string("Can not open xml file "));
            }

            QString errorMsg;
            if(!doc.setContent(&xml, nullptr, &errorMsg)) {
                qDebug() << "Error parsing xml: " << errorMsg;
                throw std::runtime_error(std::string("XML Parsing error occured."));
            }
        } catch(std::runtime_error e) {
            QFile errFile("errors.log");
            QString errStr = QDate::currentDate().toString()
                             + ": "
                             + e.what()
                             + '\n';

            if(errFile.open(QIODevice::Text | QIODevice::Append)) {
                errFile.write(errStr.toLocal8Bit());
                errFile.close();
            }


            QMessageBox::critical(this, "Error", "Error occured: " + errStr);
            exit(1);
        }

        QDomElement elem = doc.documentElement();
        traverseNode(elem, records);
        xml.close();

        QPushButton* pBtnOk = new QPushButton("OK");
        QObject::connect(pBtnOk, SIGNAL(clicked(bool)), SLOT(close()));
        // layout setup
        pScoreLayout = new QVBoxLayout(pscrollArea);
        HeaderLabelWidget* pHeader = new HeaderLabelWidget(this);
        pScoreLayout->addWidget(pHeader);

        int pos = 1;
        for(auto iter = records.end() - 1; iter != records.begin() - 1; --iter) {
            RecordLabel* lblPosition = new RecordLabel(QString::number(pos));
            lblPosition->setFont(font);
            lblPosition->setStyleSheet("RecordLabel :hover { color: red; }");
            RecordLabel* lblName = new RecordLabel(iter.value());
            lblName->setFont(font);
            lblName->setStyleSheet("RecordLabel :hover { color: red; }");
            RecordLabel* lblScore = new RecordLabel(QString::number(iter.key()));
            lblScore->setFont(font);
            lblScore->setStyleSheet("RecordLabel :hover { color: red; }");

            if(iter.value() == player && QString::number(iter.key()) == record) {
                QString stylesheet = " QLabel { color: red; } ";
                lblPosition->setStyleSheet(stylesheet);
                lblName->setStyleSheet(stylesheet);
                lblScore->setStyleSheet(stylesheet);
            }

            QHBoxLayout* precordLayout = new QHBoxLayout;
            precordLayout->addWidget(lblPosition, 0, Qt::AlignCenter);
            precordLayout->addWidget(lblName, 0, Qt::AlignCenter);
            precordLayout->addWidget(lblScore, 0, Qt::AlignCenter);

            pScoreLayout->addLayout(precordLayout);
            ++pos;
        }


        proxyWidget->setLayout(pScoreLayout);
        pscrollArea->setFrameShape(QFrame::NoFrame);
        pscrollArea->setWidgetResizable(true);
        pscrollArea->setWidget(proxyWidget);


        QVBoxLayout* pMainLayout = new QVBoxLayout(this);
        pMainLayout->addWidget(pHeader, 0, Qt::AlignTop);
        pMainLayout->addWidget(pscrollArea);
        pMainLayout->addWidget(pBtnOk, 0, Qt::AlignCenter);

        resize(640, 420);
        setLayout(pMainLayout);

    }


    void traverseNode(const QDomNode& node, QMultiMap<int, QString>& records) {
        QDomNode domNode = node.firstChild();
        while(!domNode.isNull()) {
            if(domNode.isElement()) {
                QDomElement elem = domNode.toElement();
                if(!elem.isNull()) {
                    records.insert(elem.attribute("score").toInt(),
                                   elem.attribute("name")
                                   );
                    qDebug() << "Name " << elem.attribute("name") << " score: " << elem.attribute("score");
                }
            }
            traverseNode(domNode, records);
            domNode = domNode.nextSibling();
        }
    }

    /**
     * @brief addRecord - method adds new node with attributes
     *        into xml document
     * @param name  - Name of player
     * @param score - Gained score
     * @return true if addition was successful
     *         otherwise false
     */
    bool addRecord(QString name, QString score) {
        QFile xml(xmlFilePath);

        if(name.trimmed().isEmpty()) {
            return false;
        }

        if(!xml.open(QIODevice::ReadWrite)) {
            return false;
        }

        QString scoreStr(xml.readAll());
        xml.resize(0);
        QXmlStreamReader reader(scoreStr);

        QXmlStreamWriter writer(&xml);
        writer.setAutoFormatting(true);

        bool isWritten = false;

        while(!reader.atEnd()) {
            if(reader.isStartDocument())
                writer.writeStartDocument();
            if(reader.isStartElement())
                writer.writeStartElement(reader.name().toString());

            if(reader.name() == "scores" && isWritten == false) {
                writer.writeStartElement("player");
                writer.writeAttribute("name", name);
                writer.writeAttribute("score", score);
                writer.writeEndElement();
                isWritten = true;
            }

            for(auto iter =  reader.attributes().begin(); iter != reader.attributes().end(); ++iter)
                writer.writeAttribute(*iter);

            if(reader.isCharacters())
                writer.writeCharacters(reader.text().toString());

            if(reader.isEndElement())
                writer.writeEndElement();

            if(reader.isEndDocument())
                writer.writeEndElement();

            reader.readNext();
        }

        return true;
    }

};

#endif // SCOREWINDOW_H
