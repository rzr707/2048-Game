/**
  @Окно появляется при наборе в 2048 в одной из ячеек
*/
#ifndef WINWINDOW
#define WINWINDOW

#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QPalette>
#include <QObject>

class WinWindow: public QWidget {
    Q_OBJECT
private:
    QLabel*      m_pHeadLabel;
    QLabel*      m_pBodyLabel;
    QLineEdit*   m_pPlayerName;
    QPushButton* m_pcmdOk;
    QPushButton* m_pcmdResults;

public:
    WinWindow(QWidget* pwgt = 0, uint score = 0)
        : QWidget(pwgt) {

        QFont f = font();
        f.setPointSize(20);
        setFont(f);

        m_pHeadLabel  = new QLabel("<center>You've got the 2048 tile!<center>");
        m_pHeadLabel->setMargin(0);

        m_pBodyLabel = new QLabel("Also, your score equals " +
                             QString::number(score) + " points!. You can continue playing!"
                             );
        f.setPointSize(15);
        m_pBodyLabel->setWordWrap(true);
        m_pBodyLabel->setFont(f);

        m_pPlayerName = new QLineEdit;
        m_pPlayerName->setAlignment(Qt::AlignHCenter);
        m_pPlayerName->setPlaceholderText("Enter your name, winner!");

        m_pcmdOk     = new QPushButton("YEAH!");
        m_pcmdOk->setAttribute(Qt::WA_TranslucentBackground);
        m_pcmdOk->setStyleSheet(
                                "background-color: green; "
                                "border: 1px solid black; "
                                "border-radius: 5px; "
                                "font-size: 15px; ");

        m_pcmdResults = new QPushButton("Results");
        m_pcmdResults->setAttribute(Qt::WA_TranslucentBackground);
        m_pcmdResults->setStyleSheet(
                                "background-color: yellow; "
                                "border: 1px solid black; "
                                "border-radius: 5px; "
                                "font-size: 15px; ");

        QHBoxLayout* phbl   = new QHBoxLayout;
        phbl->addWidget(m_pcmdOk);
        phbl->addWidget(m_pcmdResults);

        QVBoxLayout* pMainL = new QVBoxLayout;
        pMainL->addWidget(m_pHeadLabel, 0, Qt::AlignTop | Qt::AlignHCenter);
        pMainL->addWidget(m_pPlayerName);
        pMainL->addWidget(m_pBodyLabel, 0, Qt::AlignHCenter);
        pMainL->addLayout(phbl);

        connect(m_pcmdOk, SIGNAL(clicked(bool)),
                this,     SLOT(slotOkClicked())
                );
        connect(m_pcmdResults, SIGNAL(clicked(bool)),
                this,          SLOT(slotResultClicked())
                );

        QPalette pal;
        pal.setColor(backgroundRole(), Qt::white);
        setPalette(pal);

        setLayout(pMainL);

        setWindowFlags(Qt::WindowStaysOnTopHint);
        setFixedSize(350, 250);

    }

public slots:
    void slotOkClicked() {
        emit signalOk(m_pPlayerName->text().trimmed());
        close();
    }

    void slotResultClicked() {
        emit signalResult(m_pPlayerName->text().trimmed());
    }

signals:
    void signalOk(QString player);
    void signalResult(QString player);
};

#endif // WINWINDOW

