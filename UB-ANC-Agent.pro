#-------------------------------------------------
#
# Project created by QtCreator 2015-10-16T20:53:48
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = ub-anc-agent
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

#
# APM Planner Library
#
include(apm_planner.pri)

INCLUDEPATH += \
    agent \
    network \
    sensor \
    sensor/power \

HEADERS += \
    agent/UBAgent.h \
    network/UBNetwork.h \
    network/UBNetPacket.h \
    sensor/power/UBPWRPacket.h \
    sensor/UBVision.h \
    config.h \
    sensor/power/UBPower.h \
    network/UBConnectivity.h

SOURCES += \
    agent/UBAgent.cpp \
    network/UBNetwork.cpp \
    network/UBNetPacket.cpp \
    sensor/power/UBPWRPacket.cpp \
    sensor/UBVision.cpp \
    sensor/power/UBPower.cpp \
    network/UBConnectivity.cpp
