TARGET      = qtraductor

SOURCES += src/main.cpp \
    src/mainwindow.cpp \
    src/PLLayout.cpp
HEADERS += src/mainwindow.h \
    src/PLLayout.h
RESOURCES += src/qtraductor.qrc

TRANSLATIONS = src/qtraductor_ca_ES.ts \
    src/qtraductor_es_ES.ts

LEXSOURCES  += #LEXS#
YACCSOURCES += #YACCS#

INCLUDEPATH +=
LIBS        +=
DEFINES     +=

# All generated files goes same directory
OBJECTS_DIR = build
MOC_DIR     = build
UI_DIR      = build

DESTDIR     = build
TEMPLATE    = app
DEPENDPATH  +=
VPATH       += src uis
CONFIG      -=
CONFIG      += debug
QT=core gui network

INSTALLS    += target
target.path  = /opt/qtraductor/bin/


INSTALLS    += desktop
desktop.path  = /usr/share/applications/hildon
desktop.files  = data/qtraductor.desktop

INSTALLS    += icon64
icon64.path  = /usr/share/icons/hicolor/64x64/apps
icon64.files  = data/64x64/qtraductor.png

#
# Targets for debian source and binary package creation
#
debian-src.commands = dpkg-buildpackage -S -r -us -uc -d
debian-bin.commands = dpkg-buildpackage -b -r -uc -d
debian-all.depends = debian-src debian-bin

#
# Clean all but Makefile
#
compiler_clean.commands = -$(DEL_FILE) $(TARGET)

QMAKE_EXTRA_TARGETS += debian-all debian-src debian-bin compiler_clean


symbian {
    vendorinfo = \
    "%{\"Toni Hermoso - Softcatala\"}" \
    ":\"Toni Hermoso - Softcatala\""

    my_deployment.pkg_prerules = vendorinfo
    DEPLOYMENT += my_deployment

    TARGET = qtraductor
    TARGET.UID3 = 0xe11064e1
    TARGET.CAPABILITY += "NetworkServices"
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}
