# Project.pro - Qt Project Configuration File
# Client Management System with Oracle Database Connection

# Qt modules required
QT += core gui widgets sql

# Enable Qt's moc system and C++17
CONFIG += qt c++17 warn_on

# Target executable name
TARGET = ClientManagementSystem

# Application template
TEMPLATE = app

# Project configuration
CONFIG -= app_bundle
CONFIG -= console  # Remove for GUI applications

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
FORMS += \
    mainwindow.ui

# Resources - comment out if you don't have resources.qrc
# RESOURCES += resources

# Output directories - simplified approach
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

# Compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

# Platform-specific configurations
win32 {
    # Windows-specific configuration
    CONFIG += windows

    # Oracle Client Libraries for Windows (if using Oracle)
    # Uncomment and adjust paths if you're using Oracle
    # LIBS += -loci
    # INCLUDEPATH += "C:/oracle/client/include"
    # LIBS += -L"C:/oracle/client/lib"

    # For SQLite (default Qt SQL driver)
    # No additional libraries needed
}

unix:!macx {
    # Linux-specific configuration
    # For Oracle: LIBS += -lclntsh
    # INCLUDEPATH += /opt/oracle/instantclient_21_1/include
    # LIBS += -L/opt/oracle/instantclient_21_1/lib
}

macx {
    # macOS-specific configuration
    # For Oracle: LIBS += -lclntsh
    # INCLUDEPATH += /opt/oracle/instantclient_19_8/include
    # LIBS += -L/opt/oracle/instantclient_19_8/lib
}

# Debug and Release configurations
CONFIG(debug, debug|release) {
    DESTDIR = debug
    DEFINES += DEBUG_MODE
    QMAKE_CXXFLAGS += -g
} else {
    DESTDIR = release
    DEFINES += RELEASE_MODE
}

# Additional includes
INCLUDEPATH += $$PWD

# Minimum Qt version required
lessThan(QT_MAJOR_VERSION, 6) {
    lessThan(QT_MAJOR_VERSION, 5): error("This project requires Qt 5 or higher")
}
