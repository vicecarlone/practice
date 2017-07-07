#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "solver.h"


void setInit(Solver*);

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    Solver* solver = new Solver();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    setInit(solver);
    solver->start();
    solver->checkCell();
    solver->checkPossible();
    solver->currentValue();


    if(solver->checkCell())
        solver->checkPossible();
    else {
        solver->hiddenSingle();
        solver->checkCell();
    }
    solver->currentValue();

    return app.exec();
}

void setInit(Solver * solver){
    /* 9 5 0 1 7 6 0 0 3
     * 4 0 0 0 0 0 0 1 0
     * 3 1 0 0 4 0 8 0 9
     * 6 0 0 4 0 0 7 0 0
     * 0 0 0 7 9 5 0 0 0
     * 0 0 3 0 0 8 0 0 4
     * 7 0 9 0 5 0 0 3 8
     * 0 8 0 0 0 0 0 0 5
     * 5 0 0 8 6 4 0 9 7
     */
    UNIT(1,1,9); UNIT(1,2,5); UNIT(1,4,1); UNIT(1,5,7); UNIT(1,6,6); UNIT(1,9,3);
    UNIT(2,1,4); UNIT(2,8,1);
    UNIT(3,1,3); UNIT(3,2,1); UNIT(3,5,4); UNIT(3,7,8); UNIT(3,9,9);
    UNIT(4,1,6); UNIT(4,4,4); UNIT(4,7,7);
    UNIT(5,4,7); UNIT(5,5,9); UNIT(5,6,5);
    UNIT(6,3,3); UNIT(6,6,8); UNIT(6,9,4);
    UNIT(7,1,7); UNIT(7,3,9); UNIT(7,5,5); UNIT(7,8,3); UNIT(7,9,8);
    UNIT(8,2,8); UNIT(8,9,5);
    UNIT(9,1,5); UNIT(9,4,8); UNIT(9,5,6); UNIT(9,6,4); UNIT(9,8,9); UNIT(9,9,7);

}
