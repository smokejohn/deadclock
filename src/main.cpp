#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QDirIterator>

#include "WindowController.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    // QDirIterator it(":", QDirIterator::Subdirectories);
    // while (it.hasNext()) {
    //   qDebug() << it.next();
    // }

    app.setWindowIcon(QIcon(":/images/deadclock_icon.ico"));

    WindowController* window_controller = new WindowController(&app);

    return app.exec();
}
