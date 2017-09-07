/**
  void init(void)                - первичная инициализация массива ячеек
  bool upPressed()               - вызывается при нажатии клавиши "вниз"
  bool upMove()                  - при нажатии сдвигает ячейки
  bool upMerge()                 - сливает ячейки
  void addScore(uint score)      - добавляет очки
  void setScore(uint score)      - устанавливает очки
  void moveTile(Move m, Cell& c) - двигает ячейки (анимация) p.s. не используется (см. метод в cell.h)
  void slotMoveTile(Move m)      - двигает ячейки (анимация)
  void slotNewGame()             - сбрасывает очки и начинает игру заново
  void slotExit()                - закрывает приложение
*/
#ifndef DESK
#define DESK

#include "cell.h"
#include <random>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
#include <ctime>    // for srand(time(0))
#include <QSound>
#include <QLabel>
#include "losewindow.h"
#include "winwindow.h"
#include "scorewindow.h"
using namespace std;

class Desk : public QWidget {
    Q_OBJECT
private:
    Cell    m_arrCell[4][4];
    QSound* m_psnd;
    QSound* m_psndLose;
    QSound* m_psndWin;   //todo
    QLabel* m_plbl;
    QLabel* m_plblScore;
    QFile*  m_scoreFile;
    uint    m_nScore;
    bool    m_bIsWon;
    bool    m_isMoved;

    std::mt19937       eng;

protected:
    /*
     * Если не было сдвигов тайлов или слияния тайлов,
     * bIsRand = false ,
     * Значит не будет анимации сдвига и воспроизведения звука тайлов
     */
    virtual void keyPressEvent(QKeyEvent* pe) {
        bool bIsRand = false;

        switch(pe->key()) {
        case Qt::Key_Up:
            upPressed();
            bIsRand = randCell();
            if(bIsRand)
                signalMoveAnimation(UP);
            break;
        case Qt::Key_Down:
            downPressed();
            bIsRand = randCell();
            if(bIsRand)
                signalMoveAnimation(DOWN);
            break;
        case Qt::Key_Left:
            leftPressed();
            bIsRand = randCell();
            if(bIsRand)
                signalMoveAnimation(LEFT);
            break;
        case Qt::Key_Right:
            rightPressed();
            bIsRand = randCell();
            if(bIsRand)
                signalMoveAnimation(RIGHT);
            break;
        }

        if(bIsRand)
            m_psnd->play();

        if(!m_bIsWon) {
            winCheck();
        }
    }

    // подгоняет размер шрифтов при изменении размеров окна
    virtual void resizeEvent(QResizeEvent*) {
        QFont font = m_plbl->font();
        font.setFamily("poor richard");
        font.setPointSize(width() / 20);
        setFont(font);
        m_plbl->setFont(font);
        m_plblScore->setFont(font);
    }

public:
    Desk(QWidget* pwgt = 0)
        : QWidget(pwgt),
          m_scoreFile(new QFile(QApplication::applicationDirPath() + "\\scores.xml")),
          m_nScore(0),
          m_bIsWon(false) {

        QFont font = this->font();
        font.setBold(true);
        font.setPointSize(15);

        m_psnd      = new QSound(":/sounds/res/sound/snd1.wav");
        m_psndLose  = new QSound(":/sounds/res/sound/lose.wav");
        m_psndWin   = new QSound(":/sounds/res/sound/win.wav");
        m_plbl      = new QLabel("SCORE: ");
        m_plbl->setMargin(0);
        m_plblScore = new QLabel(QString::number(m_nScore));
        m_plbl->setFont(font);
        m_plblScore->setFont(font);

        QHBoxLayout* phbl = new QHBoxLayout;

        phbl->addWidget(m_plbl, 0, Qt::AlignHCenter);
        phbl->addWidget(m_plblScore, 0, Qt::AlignHCenter);
        phbl->setSpacing(0);
        phbl->setMargin(0);

        qDebug() << "constructor called";
        init();
        QGridLayout* pgl = new QGridLayout;
        pgl->addLayout(phbl, 0, 0, 1, 4, Qt::AlignTop | Qt::AlignHCenter);
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                pgl->addWidget(&m_arrCell[i][j], i+1, j);
              }
        }

        QVBoxLayout* pMainLayout = new QVBoxLayout;
        pMainLayout->setAlignment(phbl, Qt::AlignTop);
        pMainLayout->addLayout(pgl);

        setLayout(pMainLayout);
        resize(480, 540);

        setAutoFillBackground(true);
        setBackgroundRole(QPalette::Background);
        QPalette pal;
        pal.setColor(QPalette::Background, Qt::white);
        setPalette(pal);

        // random generator setup:
        std::random_device rd;
        eng.seed(rd());

        connect(this, SIGNAL(signalMoveAnimation(Move)),
                this, SLOT(slotMoveTile(Move))
                );

        //m_arrCell[0][0].setNumber(2048); // for font testing

    }

    /* Добавляет очки */
    void addScore(uint score) {
        m_nScore += score;
        qDebug() << "m_nScore is: " << QString::number(m_nScore);
        m_plblScore->setText(QString::number(m_nScore));
        qDebug() << "window size: " << this->size();
    }

    void setScore(uint score) {
        m_nScore = score;
        m_plblScore->setText(QString::number(m_nScore));
    }

    void init(void) {

        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                m_arrCell[i][j].setNumber(0);
            }
        }
        uint x = 0;
        /* Рандом */
        std::uniform_int_distribution<int> distr(0, 3);

        /* Выбирает две случайные ячейки и заливает туда 2 */
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                if(m_arrCell[i][j].getNumber() != 0)
                    return;
                if(x < 2) {
                    m_arrCell[distr(eng)][distr(eng)].setNumber(2);
                    ++x;
                }
            }
        }
        qDebug() << "init called";
    }

    /* Рандомно втыкает в одну из нулевых ячеек 2 или 4 */
    bool randCell(void) {
        // проверка на заполненность всех ячеек:
        bool bIsFilled = true;
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                if(!m_arrCell[i][j].getNumber()) {
                    bIsFilled = false;
                }
            }
        }

        if(bIsFilled) {
            if(     // if the field is filled and there is no merge moves
                    // it means that the game is over
                       !isLeftMovable()
                    && !isRightMovable()
                    && !isUpMovable()
                    && !isDownMovable()
                    ) {
                m_bIsWon = false;
                m_psndLose->play();
                LoseWindow wnd(0, m_nScore);
                connect(&wnd, SIGNAL(signalOk(QString)),
                        this, SLOT(slotNewGame(QString))
                        );
                connect(&wnd, SIGNAL(signalCancel(QString)),
                        this, SLOT(slotExit(QString))
                        );
                connect(&wnd, SIGNAL(signalResults(QString)),
                        this, SLOT(slotResults(QString))
                        );

                wnd.exec();

                return true;
            }
        }

        // if none of tiles moved or merged than do nothing:
        if(!m_isMoved)
            return false;
        m_isMoved = false;

        bool b = true;
        while(b) {
            std::uniform_int_distribution<int> distr(0, 3);
            uint randA = distr(eng);
            uint randB = distr(eng);

            srand(time(0));

            if(m_arrCell[randA][randB].getNumber() == 0) {
                qDebug() << "m_arrCell[distr(eng)][distr(eng)].getNumber() == 0";
                qDebug() << "randA = " << randA << " randB = " << randB;
                m_arrCell[randA][randB].setNumber((rand() % 4 == 0) ? 4 : 2);
                b = false;
            }
        }

        qDebug() << "randCell called";
        return true;
    }

    /*
    Алгоритм сдвига значений ячеек:
    вниз:
      все ячейки столбца сдвигаются на 1 ниже (кроме последней), если там НЕ пусто
    */
    bool upMove() {
        bool isMovedOrMerged = false;
        for(int i = 0; i < 4; ++i) {
            int j = 1;
            while(j < 4) {
                if(m_arrCell[j][i].getNumber() == 0) {
                    ++j;
                } else if(m_arrCell[j-1][i].getNumber() == 0) {
                    m_arrCell[j-1][i].setNumber(m_arrCell[j][i].getNumber());
                    m_arrCell[j][i].setNumber(0);
                    isMovedOrMerged = true;
                    if(j - 1 > 0)
                        --j;
                } else if(m_arrCell[j-1][i].getNumber() == m_arrCell[j][i].getNumber()) {
                    m_arrCell[j-1][i].setNumber(m_arrCell[j-1][i].getNumber() * 2);
                    addScore(m_arrCell[j-1][i].getNumber());
                    m_arrCell[j][i].setNumber(0);
                    isMovedOrMerged = true;
                    ++j;
                } else ++j;
            }
        }
        return isMovedOrMerged;
    }

    void upPressed() {
       m_isMoved = upMove();
    }

    bool downMove() {
        bool isMovedOrMerged = false;
        for(int i = 0; i < 4; ++i) {
            int j = 2;
            while(j >= 0) {
                if(m_arrCell[j][i].getNumber() == 0) {
                    --j;
                } else if(m_arrCell[j+1][i].getNumber() == 0) {
                    m_arrCell[j+1][i].setNumber(m_arrCell[j][i].getNumber());
                    m_arrCell[j][i].setNumber(0);
                    isMovedOrMerged = true;
                    if(j + 1 < 3)
                        ++j;
                } else if(m_arrCell[j+1][i].getNumber() == m_arrCell[j][i].getNumber()) {
                    m_arrCell[j+1][i].setNumber(m_arrCell[j+1][i].getNumber() * 2);
                    addScore(m_arrCell[j+1][i].getNumber());
                    m_arrCell[j][i].setNumber(0);
                    isMovedOrMerged = true;
                    --j;
                } else --j;
            }
        }
        return isMovedOrMerged;
    }

    void downPressed() {
         m_isMoved = downMove();
    }

    bool leftMove() {
        bool isMovedOrMerged = false;
        for(int i = 0; i < 4; ++i) {
            int j = 1;
            while(j < 4) {
                if(m_arrCell[i][j].getNumber() == 0) {
                    ++j;
                } else if(m_arrCell[i][j-1].getNumber() == 0) {
                    m_arrCell[i][j-1].setNumber(m_arrCell[i][j].getNumber());
                    m_arrCell[i][j].setNumber(0);
                    isMovedOrMerged = true;
                    if(j - 1 > 0)
                        --j;
                } else if(m_arrCell[i][j-1].getNumber() == m_arrCell[i][j].getNumber()) {
                    m_arrCell[i][j-1].setNumber(m_arrCell[i][j-1].getNumber() * 2);
                    addScore(m_arrCell[i][j-1].getNumber());
                    m_arrCell[i][j].setNumber(0);
                    isMovedOrMerged = true;
                    ++j;
                } else ++j;
            }
        }
        return isMovedOrMerged;
    }

    void leftPressed() {
        m_isMoved = leftMove();
    }

    bool rightMove() {
        bool isMovedOrMerged = false;
        for(int i = 0; i < 4; ++i) {
            int j = 2;
            while(j >= 0) {
                if(m_arrCell[i][j].getNumber() == 0) {
                    --j;
                } else if(m_arrCell[i][j+1].getNumber() == 0) {
                    m_arrCell[i][j+1].setNumber(m_arrCell[i][j].getNumber());
                    m_arrCell[i][j].setNumber(0);
                    isMovedOrMerged = true;
                    if(j + 1 < 3)
                        ++j;
                } else if(m_arrCell[i][j+1].getNumber() == m_arrCell[i][j].getNumber()) {
                    m_arrCell[i][j+1].setNumber(m_arrCell[i][j+1].getNumber() * 2);
                    addScore(m_arrCell[i][j+1].getNumber());
                    m_arrCell[i][j].setNumber(0);
                    isMovedOrMerged = true;
                    --j;
                } else --j;
            }
        }
        return isMovedOrMerged;
    }

    void rightPressed() {
        m_isMoved = rightMove();
    }


    bool isLeftMovable() {
        for(int i = 0; i < 4; ++i) {
            for(int j = 1; j < 4; ++j) {
                if(m_arrCell[i][j].getNumber() == m_arrCell[i][j-1].getNumber()) {
                    return true;
                }
            }
        }

        return false;
    }

    bool isRightMovable() {
        for(int i = 0; i < 4; ++i) {
            for(int j = 2; j >= 0; --j) {
                if(m_arrCell[i][j].getNumber() == m_arrCell[i][j+1].getNumber()) {
                    return true;
                }
            }
        }

        return false;
    }

    bool isUpMovable() {
        for(int i = 0; i < 4; ++i) {
            for(int j = 1; j < 4; ++j) {
                if(m_arrCell[j][i].getNumber() == m_arrCell[j-1][i].getNumber()) {
                    return true;
                }
            }
        }

        return false;
    }

    bool isDownMovable() {
        for(int i = 0; i < 4; ++i) {
            for(int j = 2; j >= 0; --j) {
                if(m_arrCell[j][i].getNumber() == m_arrCell[j+1][i].getNumber()) {
                    return true;
                }
            }
        }

        return false;
    }

public slots:
    /*см метод в cell.h*/
    void slotMoveTile(Move m)
    {
        qDebug() << "slotMoveTile called";
        switch(m)
        {
        case LEFT:
            for(int i = 0; i < 4; ++i) {
                for(int j = 1; j < 4; ++j) {
                    m_arrCell[i][j].moveCell(LEFT);
                }
            }
            break;
        case RIGHT:
            for(int i = 0; i < 4; ++i) {
                for(int j = 0; j < 3; ++j) {
                    m_arrCell[i][j].moveCell(RIGHT);
                }
            }
            break;
        case UP:
            for(int i = 1; i < 4; ++i) {
                for(int j = 0; j < 4; ++j) {
                    m_arrCell[i][j].moveCell(UP);
                }
            }
            break;
        case DOWN:
            for(int i = 0; i < 3; ++i) {
                for(int j = 0; j < 4; ++j) {
                    m_arrCell[i][j].moveCell(DOWN);
                }
            }
            break;
        }
    }

    void slotNewGame(QString player) {
       ScoreWindow* pScoreWnd = new ScoreWindow(*m_scoreFile, player, QString::number(m_nScore));
       delete pScoreWnd;

       qobject_cast<QWidget*>(sender())->close(); // close signal sender

       if(!m_bIsWon) {
          setScore(0);
          init();
       }
    }

    void slotExit(QString player) {
        ScoreWindow* pScoreWnd = new ScoreWindow(*m_scoreFile, player, QString::number(m_nScore));
        delete pScoreWnd;

        qobject_cast<QWidget*>(sender())->close();

        qApp->quit();
    }

    void slotResults(QString player) {
        ScoreWindow* pScoreWnd = new ScoreWindow(*m_scoreFile, player, QString::number(m_nScore));
        pScoreWnd->show();

        qobject_cast<QWidget*>(sender())->close();
        if(!m_bIsWon) {
           setScore(0);
           init();
        }
    }

    void winCheck() {
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                if(m_arrCell[i][j].getNumber() == 2048) {
                    WinWindow* wnd = new WinWindow(0, m_nScore);
                    connect(wnd, SIGNAL(signalOk(QString)),
                            this, SLOT(slotNewGame(QString))
                            );
                    connect(wnd, SIGNAL(signalResult(QString)),
                            this, SLOT(slotResults(QString))
                            );
                    wnd->show();
                    m_bIsWon = true;
                }
            }
        }
    }

signals:
    void signalMoveAnimation(Move m);

};

#endif // DESK

