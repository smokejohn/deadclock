#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QDirIterator>

#include "Application.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    // QDirIterator it(":", QDirIterator::Subdirectories);
    // while (it.hasNext()) {
    //   qDebug() << it.next();
    // }

    app.setWindowIcon(QIcon(":/images/deadclock_icon.ico"));

    Application* deadclock = new Application(&app);

    return app.exec();
}
