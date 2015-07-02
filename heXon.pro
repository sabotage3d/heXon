LIBS += /home/frode/Sync/Source/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -lSDL \
    -ldl \
    -lGL

DEFINES += URHO3D_COMPILE_QT

QMAKE_CXXFLAGS += -std=c++11



INCLUDEPATH += \
    /home/frode/Sync/Source/Urho3D/include \
    /home/frode/Sync/Source/Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    mastercontrol.cpp \
    inputmaster.cpp \
    tile.cpp \
    tilemaster.cpp \
    hexocam.cpp \
    pickup.cpp \
    sceneobject.cpp \
    enemy.cpp \
    razor.cpp \
    player.cpp \
    apple.cpp \
    heart.cpp \
    bullet.cpp \
    arenaedge.cpp \
    flash.cpp \
    hitfx.cpp \
    effect.cpp \
    explosion.cpp \
    spawnmaster.cpp \
    spire.cpp \
    objectpool.cpp \
    muzzle.cpp \
    seeker.cpp

HEADERS += \
    mastercontrol.h \
    inputmaster.h \
    tile.h \
    tilemaster.h \
    hexocam.h \
    pickup.h \
    sceneobject.h \
    enemy.h \
    razor.h \
    player.h \
    apple.h \
    heart.h \
    bullet.h \
    arenaedge.h \
    flash.h \
    hitfx.h \
    effect.h \
    explosion.h \
    spawnmaster.h \
    spire.h \
    objectpool.h \
    muzzle.h \
    seeker.h
