#include <QDirIterator>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QFontDatabase>
#include <qfontdatabase.h>

#include "Application.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    // QDirIterator it(":", QDirIterator::Subdirectories);
    // while (it.hasNext()) {
    //   qDebug() << it.next();
    // }

    // auto regular_font_id = QFontDatabase::addApplicationFont(":/fonts/OpenSans-Regular.ttf");
    // auto semibold_font_id = QFontDatabase::addApplicationFont(":/fonts/OpenSans-SemiBold.ttf");
    // auto regular_font_id = QFontDatabase::addApplicationFont(":/fonts/Roboto-Regular.ttf");
    // auto semibold_font_id = QFontDatabase::addApplicationFont(":/fonts/Roboto-SemiBold.ttf");
    auto regular_font_id = QFontDatabase::addApplicationFont(":/fonts/Lato-Regular.ttf");
    auto semibold_font_id = QFontDatabase::addApplicationFont(":/fonts/Lato-Bold.ttf");
    QString regular_font = QFontDatabase::applicationFontFamilies(regular_font_id).at(0);
    QString semibold_font = QFontDatabase::applicationFontFamilies(semibold_font_id).at(0);

    QFont default_font(regular_font);
    default_font.setPointSize(12);
    app.setFont(default_font);

    app.setWindowIcon(QIcon(":/images/deadclock_icon.ico"));

    Application* deadclock = new Application(&app);

    return app.exec();
}
