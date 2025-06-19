QT += quick
CONFIG += c++11
DEFINES += PLATFORM_QT

INCLUDEPATH += common hal/sim services logic

SOURCES += \
    hal/sim/led_sim.c \
    hal/sim/us_sim.c \
    hal/sim/batt_sim.c \
    hal/sim/touch_sim.c \
    hal/sim/time_sim.c \
    services/soft_timer.c \
    services/anim.c \
    logic/fsm.c \
    sim/main_sim.cpp \
    sim/simviewmodel.cpp

HEADERS += \
    common/platform.h \
    common/types.h \
    hal/battery.h \
    hal/led.h \
    hal/time.h \
    hal/touch.h \
    hal/ultrasonic.h \
    include/stdbool.h \
    logic/fsm.h \
    services/anim.h \
    services/soft_timer.h \
    sim/simviewmodel.h


RESOURCES += qt/qml/qml.qrc

# Default rules for deployment.
DESTDIR = $$PWD/bin

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
