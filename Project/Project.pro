# Project.pro - Qt Project Configuration File
# Client Management System with Oracle Database Connection

# Qt modules required - FIXED: Added missing modules
QT += core gui widgets sql printsupport network

# Enable C++17 standard - FIXED: Added for better compatibility
CONFIG += c++17

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
    commandswindow.cpp \
    chatbotdialog.cpp

# Header files (.h)
HEADERS += \
    clients.h \
    commands.h \
    connection.h \
    mainwindow.h \
    clientswindow.h \
    commandswindow.h \
    chatbotdialog.h \
    emailservice.h

# UI files (.ui) - FIXED: Added both UI files
FORMS += \
    mainwindow.ui \
    chatbotdialog.ui

# Compiler flags
QMAKE_CXXFLAGS += -Wall -Wextra

# FIXED: Remove problematic Release flags that might conflict
# QMAKE_CXXFLAGS_RELEASE += -O2

# Platform-specific configurations
win32 {
    # Windows-specific configuration
    CONFIG += windows
    QMAKE_CXXFLAGS += -DUNICODE -D_UNICODE

    # FIXED: Add Windows-specific libraries for network functionality
    LIBS += -lws2_32 -lwsock32
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

# FIXED: Enable SSL support for email functionality
QT += network
CONFIG += openssl

# Output directories
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
UI_DIR = build/ui

# Disable precompiled headers
CONFIG -= precompile_header

# FIXED: Ensure proper linking order
CONFIG += link_pkgconfig

# Add resources if you have any (uncomment if needed)
# RESOURCES += resources.qrc
