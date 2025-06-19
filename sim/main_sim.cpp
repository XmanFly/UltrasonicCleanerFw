#include <stdio.h>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLoggingCategory>
#include <QFont>
#include <QDateTime>
#include <QTimer>
#include <QThread>
#include "common/platform.h"
#include "hal/ultrasonic.h"
#include "logic/fsm.h"
#include "hal/battery.h"
#include "hal/touch.h"
#include "hal/time.h"
#include "services/soft_timer.h"
#include "sim/simviewmodel.h"

 void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
 {
     QDateTime time = QDateTime::currentDateTime();
     QString out = time.toString("[yyyy-MM-dd hh:mm:ss.zzz]") +
             "[" + QString(context.category) + "]" +
             "[" + QString(context.function) + "]"+ msg;

     switch (type) {
     case QtDebugMsg:
         break;
     case QtInfoMsg:
     case QtWarningMsg:
     case QtCriticalMsg:
     case QtFatalMsg:
         break;
     }
     fprintf(stdout, "%s\n", out.toLocal8Bit().constData());
     fflush(stdout);
 }

void fwInit(QObject *parent, SimViewModel *simViewModel)
{
    // 注册回调
    hal_us_set_callback(simViewModel->onHalUsCallback);

    hal_time_init();
    fsm_init();

    QTimer *loopTimer = new QTimer(parent);
    loopTimer->callOnTimeout([](){
        fsm_loop();
    });
    loopTimer->start(0);

    QTimer *tick1MsTimer = new QTimer(parent);
    tick1MsTimer->callOnTimeout([](){
        hal_time_tick_1ms();
    });
    tick1MsTimer->setTimerType(Qt::TimerType::PreciseTimer);
    tick1MsTimer->start(1);

}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    qInstallMessageHandler(myMessageOutput);
    QGuiApplication app(argc, argv);

    QFont font = app.font();
    font.setPointSize(14);
    app.setFont(font);

    QLoggingCategory::setFilterRules("\n"
                                      "qt.quick.tableview.*=true");

    SimViewModel simViewModel;
    fwInit(&app, &simViewModel);


    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("SimViewModel", &simViewModel);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
