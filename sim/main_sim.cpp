#include <stdio.h>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QLoggingCategory>
#include <QFont>
#include "common/platform.h"
#include "logic/fsm.h"
#include "hal/battery.h"
#include "hal/touch.h"
#include "hal/time.h"

//int main(void){
//    char ch;
//    hal_time_init();
//    fsm_init();
//    printf("Sim: t=touch, vxxxx=set mv, c=chg toggle, q=quit\n");
////    while(1){
////        if(ch=='q') break;
////        if(ch=='t') hal_touch_sim_trigger();
////        else if(ch=='c') hal_batt_sim_set_chg(!hal_batt_is_chg());
////        else if(ch=='v') {
////            char buf[6];
////            fgets(buf,6,stdin);
////            int v=atoi(buf);
////            hal_batt_sim_set_mv(v);
////        }
////    }
//    return 0;
//}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QFont font = app.font();
    font.setPointSize(14);
    app.setFont(font);

    QLoggingCategory::setFilterRules("\n"
                                      "qt.quick.tableview.*=true");


    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
