QT += core
QT += network
QT -= gui

TARGET = tcpRelay
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp
SOURCES += server.cpp

HEADERS += server.h
