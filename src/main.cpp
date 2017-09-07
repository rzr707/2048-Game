#include <QApplication>
#include <QWidget>
#include <QIcon>
#include "desk.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setApplicationName("2048");
    app.setWindowIcon(QIcon(":/res/icons/2048_icon.ico"));

    Desk game;
    game.show();

    return app.exec();
}
