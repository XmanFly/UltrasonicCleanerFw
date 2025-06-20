#include <stdio.h>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QLoggingCategory>
#include <QFont>
#include <QDateTime>
#include <QTimer>
#include <QThread>
#include <QObject>
#include "common/platform.h"
#include "hal/ultrasonic.h"
#include "logic/fsm.h"
#include "hal/battery.h"
#include "hal/touch.h"
#include "hal/time.h"
#include "services/soft_timer.h"
#include "sim/simviewmodel.h"
#include "sim/timerworker.h"

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
    hal_time_init();
    fsm_init();

    // 注册回调
    hal_us_set_callback(simViewModel->onHalUsCallback);
    fsm_set_callback(simViewModel->onEnterFsmStateCallback);

    QTimer *loopTimer = new QTimer(parent);
    loopTimer->callOnTimeout([](){
        fsm_loop();
    });
    loopTimer->start(0);

    auto tick1MsTimer = new TimerWorker(1);
    auto tick1MsTh     = new QThread;
    tick1MsTimer->moveToThread(tick1MsTh);
    QObject::connect(tick1MsTh, &QThread::started, tick1MsTimer, &TimerWorker::init);
    QObject::connect(tick1MsTimer, &TimerWorker::tick,
                     [](){
        hal_time_tick_1ms();
    }); // QueuedConnection
    tick1MsTh->start();
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
