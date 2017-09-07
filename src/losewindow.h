/**
  @Окно появляется при заполненности всех ячеек (невозможность сгенерировать число в свободную ячейку)
  TODO:
  - Написать алгоритм, который проверяет на возможность дальнейшее слияние ячеек, если слияние возможно -
    не высвечивать окно о проигрыше.
  - Сделать сериализуемую таблицу рекордов
*/
#ifndef LOSEWINDOW
#define LOSEWINDOW

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPalette>
#include <QDialog>
#include <QLineEdit>

class LoseWindow: public QDialog {
    Q_OBJECT
private:
    QLabel*      m_pHeadLabel;
    QLabel*      m_pBodyLabel;
    QLineEdit*   m_pPlayerName;
    QPushButton* m_pcmdOk;
    QPushButton* m_pcmdCancel;
    QPushButton* m_pcmdResults;

public:
    LoseWindow(QDialog* pwgt = 0, uint score = 0)
        : QDialog(pwgt) {

        QFont f = font();
        f.setPointSize(20);
        setFont(f);

        m_pHeadLabel  = new QLabel("<center>What a shame! You lose!<center>");
        m_pHeadLabel->setMargin(0);

        m_pBodyLabel = new QLabel("However, you have reached the highest score! (" +
                             QString::number(score) + " points). Want to start again?"
                             );

        m_pPlayerName = new QLineEdit(this);
        m_pPlayerName->setAlignment(Qt::AlignHCenter);
        m_pPlayerName->setPlaceholderText("Enter your name!");

        f.setPointSize(15);
        m_pBodyLabel->setWordWrap(true);
        m_pBodyLabel->setFont(f);

        QVBoxLayout* pMainL = new QVBoxLayout;
        pMainL->addWidget(m_pHeadLabel, 0, Qt::AlignTop | Qt::AlignHCenter);
        pMainL->addWidget(m_pPlayerName, 0, Qt::AlignHCenter);
        pMainL->addWidget(m_pBodyLabel, 0, Qt::AlignHCenter);

        m_pcmdOk     = new QPushButton("Yeah!");
        m_pcmdOk->setAttribute(Qt::WA_TranslucentBackground);
        m_pcmdOk->setStyleSheet("background-color: green; "
                                "border: 1px solid black; "
                                "border-radius: 5px; "
                                "font-size: 15px; ");

        m_pcmdResults = new QPushButton("Results");
        m_pcmdResults->setAttribute(Qt::WA_TranslucentBackground);
        m_pcmdResults->setStyleSheet("background-color: yellow; "
                                     "border: 1px solid black; "
                                     "border-radius: 5px; "
                                     "font-size: 15px; ");

        m_pcmdCancel = new QPushButton("Nope..");
        m_pcmdCancel->setAttribute(Qt::WA_TranslucentBackground);
        m_pcmdCancel->setStyleSheet("background-color: darkRed; "
                                    "border: 1px solid black; "
                                    "border-radius: 3px; "
                                    "font-size: 15px; ");

        QHBoxLayout* phbl = new QHBoxLayout;
        phbl->addWidget(m_pcmdOk);
        phbl->addWidget(m_pcmdCancel);
        phbl->addWidget(m_pcmdResults);

        connect(m_pcmdOk, SIGNAL(clicked(bool)),
                this,     SLOT(slotOkClicked())
                );
        connect(m_pcmdCancel, SIGNAL(clicked(bool)),
                this,         SLOT(slotCancelClicked())
                );
        connect(m_pcmdResults, SIGNAL(clicked(bool)),
                this,          SLOT(slotResults())
                );

        QPalette pal;
        pal.setColor(backgroundRole(), Qt::white);
        setPalette(pal);

        pMainL->addLayout(phbl);
        setLayout(pMainL);

        setWindowFlags(Qt::WindowStaysOnTopHint);
        setFixedSize(350, 250);

    }

public slots:
    void slotOkClicked() {
        emit signalOk(m_pPlayerName->text().trimmed());
        close();
    }

    void slotCancelClicked() {
        emit signalCancel(m_pPlayerName->text().trimmed());
        close();
    }

    void slotResults() {
        emit signalResults(m_pPlayerName->text().trimmed());
    }

signals:
    void signalOk(QString);
    void signalCancel(QString);
    void signalResults(QString); // QString is player's name
};

#endif // LOSEWINDOW

