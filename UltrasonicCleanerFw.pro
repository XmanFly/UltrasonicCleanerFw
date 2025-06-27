QT += quick
CONFIG += c++11
DEFINES += PLATFORM_QT

INCLUDEPATH += common hal/sim services logic

# 对于发布构建，添加宏定义以启用日志上下文
release: DEFINES += QT_MESSAGELOGCONTEXT

SOURCES += \
    hal/sim/led_sim.c \
    hal/sim/power_sim.c \
    hal/sim/us_sim.c \
    hal/sim/batt_sim.c \
    hal/sim/touch_sim.c \
    hal/sim/time_sim.c \
    services/led_group.c \
    services/led_sm.c \
    services/soft_pwm.c \
    services/soft_timer.c \
    logic/fsm.c \
    services/touch_service.c \
    sim/datalogger.cpp \
    sim/main_sim.cpp \
    sim/simviewmodel.cpp \
    sim/waveemitter.cpp

HEADERS += \
    common/platform.h \
    common/types.h \
    hal/battery.h \
    hal/led.h \
    hal/power.h \
    hal/time.h \
    hal/touch.h \
    hal/ultrasonic.h \
    include/stdbool.h \
    logic/fsm.h \
    services/led_group.h \
    services/led_sm.h \
    services/soft_pwm.h \
    services/soft_timer.h \
    services/touch_service.h \
    sim/datalogger.h \
    sim/simviewmodel.h \
    sim/timerworker.h \
    sim/waveemitter.h


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
