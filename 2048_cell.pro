QT += widgets multimedia xml

SOURCES += \
    src/main.cpp

HEADERS += \
    src/cell.h \
    src/desk.h \
    src/headerlabel.h \
    src/losewindow.h \
    src/recordlabel.h \
    src/scorewindow.h \
    src/winwindow.h

DISTFILES += \
    bugs.txt \
    res/icons/icon.rc

RESOURCES += \
    rs.qrc
RC_FILE = res/icons/icon.rc


CONFIG += c++11
