#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QApplication>
#include <QScopedPointer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QScopedPointer<QQmlApplicationEngine> engine;
    engine.reset(new QQmlApplicationEngine);

    app.setOrganizationName("masternuts");
    app.setApplicationName("MasterDrawing");

    const QUrl url("qrc:/ui/Main.qml");
    engine->load(url);

    return app.exec();
}
