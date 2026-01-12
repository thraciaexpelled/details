TEMPLATE = app
TARGET = details

QT = core gui

QMAKE_CXXFLAGS += -lfmt -std=c++20

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += \
    main.cpp

HEADERS +=

DISTFILES += \
    LICENSE \
    README.md \
    install.sh
