# Project.pro - Qt Project Configuration File
# Client Management System with Oracle Database Connection

# Qt modules required - let qmake handle the linking automatically
QT += core gui widgets sql

# Enable Qt's moc system
CONFIG += qt warn_on

# Target executable name
TARGET = ClientManagementSystem

# Application template
TEMPLATE = app

# Project configuration
CONFIG -= app_bundle
CONFIG -= console

# Version information
VERSION = 1.0.0

# Define application information
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# Source files (.cpp)
SOURCES += \
    main.cpp \
    clients.cpp \
    commands.cpp \
    connection.cpp \
    mainwindow.cpp \
    clientswindow.cpp \
    commandswindow.cpp

# Header files (.h)
HEADERS += \
    clients.h \
    commands.h \
    connection.h \
    mainwindow.h \
    clientswindow.h \
    commandswindow.h

# UI files (.ui)
FORMS += mainwindow.ui

# Compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra
QMAKE_CXXFLAGS_RELEASE += -O2

# Platform-specific configurations
win32 {
    # Windows-specific configuration
    CONFIG += windows
    QMAKE_CXXFLAGS += -DUNICODE -D_UNICODE
}

# Debug and Release configurations
CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,_debug)
    DEFINES += DEBUG_MODE
    QMAKE_CXXFLAGS += -g
} else {
    DEFINES += RELEASE_MODE
}

# Additional includes
INCLUDEPATH += $$PWD

# Enable exceptions
CONFIG += exceptions

# Output directories
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
UI_DIR = build/ui

# Remove manual library linking - qmake will handle this automatically
# DO NOT USE: LIBS += -lQtCore -lQtGui -lQtWidgets -lQtSql

# Disable precompiled headers
CONFIG -= precompile_header
QT += printsupport
