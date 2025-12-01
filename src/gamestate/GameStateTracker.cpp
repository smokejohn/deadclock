#include "GameStateTracker.h"
#include <QFile>
#include <QGuiApplication>
#include <QScreen>
#include <qguiapplication.h>

GameStateTracker::GameStateTracker(QObject* parent)
    : QObject(parent)
    , cv_manager(new CVManager(this))
    , ocr_manager(new OCRManager(this))
    , scan_timer(new QTimer(this))
    , primary_screen(QGuiApplication::primaryScreen())
{
    regions = new Regions(primary_screen->geometry().width(), primary_screen->geometry().height(), this);
    scan_timer->setInterval(scan_interval);

    connect(scan_timer, &QTimer::timeout, this, &GameStateTracker::scan_gamestate);
}

void GameStateTracker::start_tracking()
{
    scan_timer->start();
}

void GameStateTracker::stop_tracking()
{
    scan_timer->stop();
}

void GameStateTracker::set_scan_interval(unsigned int seconds)
{
    scan_interval = seconds * 1000;
    scan_timer->setInterval(scan_interval);
}

void GameStateTracker::reset_gamestate()
{
    gamestate = GameState();
}

void GameStateTracker::set_debug_tracking(bool debug)
{
    qDebug() << "Setting debugging gamestate tracking: " << debug;
    debug_tracking = debug;
}

QImage GameStateTracker::capture_region(QRect region)
{
    return primary_screen->grabWindow(0, region.left(), region.top(), region.width(), region.height()).toImage();
}

void GameStateTracker::update_screen_captures()
{
    timer_capture = capture_region(regions->timer_region);
    souls_team_capture = capture_region(regions->souls_team_region);
    souls_enemy_capture = capture_region(regions->souls_enemy_region);

    rejuv_team_capture = capture_region(regions->rejuv_team_region);
    rejuv_enemy_capture = capture_region(regions->rejuv_enemy_region);
    rejuv_team_tab_capture = capture_region(regions->rejuv_team_tab_region);
    rejuv_enemy_tab_capture = capture_region(regions->rejuv_enemy_tab_region);
    esc_icon_capture = capture_region(regions->esc_icon_region);
}

void GameStateTracker::scan_gamestate()
{
    update_screen_captures();
    number_of_scans++;

    gamestate.shop_open = cv_manager->is_shop_open(esc_icon_capture);

    if (gamestate.shop_open) {
        qDebug() << "Shop or Menu is open, cannot update gamestate";
        save_screen_capture(esc_icon_capture, "esc_icon");
        return;
    }
    auto gametime = ocr_manager->read_gametime(timer_capture);
    auto [souls_team, souls_enemy] = ocr_manager->read_souls(souls_team_capture, souls_enemy_capture);

    // TODO: introduce check if we are in a match
    if (gametime == -1 && souls_team == -1 && souls_enemy == -1 && !debug_tracking) {
        qDebug() << "Couldn't parse time and souls, probably not in a game";
        return;
    }

    gamestate.gametime = gametime;
    emit time_read(gamestate.gametime);

    gamestate.souls_team = souls_team;
    gamestate.souls_enemy = souls_enemy;

    auto [rejuv_team, rejuv_enemy] = cv_manager->detect_rejuv_buff(
        rejuv_team_capture, rejuv_team_tab_capture, rejuv_enemy_capture, rejuv_enemy_tab_capture);
    if (rejuv_team > gamestate.rejuv_buff_team) {
        emit rejuv_buff_team_changed(true);
    }
    if (gamestate.rejuv_buff_team > 0 && rejuv_team == 0) {
        emit rejuv_buff_team_changed(false);
    }
    if (rejuv_enemy > gamestate.rejuv_buff_enemy) {
        emit rejuv_buff_enemy_changed(true);
    }
    if (gamestate.rejuv_buff_enemy > 0 && rejuv_enemy == 0) {
        emit rejuv_buff_enemy_changed(false);
    }

    gamestate.rejuv_buff_team = rejuv_team;
    gamestate.rejuv_buff_enemy = rejuv_enemy;

    qDebug() << "Gamestate:";
    qDebug() << "gametime: " << gamestate.gametime;
    qDebug() << "shop open: " << gamestate.shop_open;
    qDebug() << "souls: " << gamestate.souls_team << " | " << gamestate.souls_enemy;
    qDebug() << "rejuv: " << gamestate.rejuv_buff_team << " | " << gamestate.rejuv_buff_enemy;

    if (debug_tracking) {
        if (!debug_dir.exists()) {
            debug_dir.mkpath(".");
        }
        // Write gamestate to .txt file for this scan
        QString gamestate_string = QString("gametime: %1\nshop open: %2\nsouls: %3 | %4\nrejuv: %5 | %6")
                                       .arg(QString::number(gamestate.gametime),
                                            gamestate.shop_open ? "true" : "false",
                                            QString::number(gamestate.souls_team),
                                            QString::number(gamestate.souls_enemy),
                                            QString::number(gamestate.rejuv_buff_team),
                                            QString::number(gamestate.rejuv_buff_enemy));

        QFile file(debug_dir.path() + "/" + QString::number(number_of_scans) + "_gamestate.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qDebug() << "Could not open file for writing:" << file.errorString();
            return;
        }
        QTextStream out(&file);
        out << gamestate_string;
        file.close();

        save_screen_capture(timer_capture, "timer");
        save_screen_capture(souls_team_capture, "souls_team");
        save_screen_capture(souls_enemy_capture, "souls_enemy");
        save_screen_capture(rejuv_team_capture, "rejuv_team");
        save_screen_capture(rejuv_team_tab_capture, "rejuv_team_tab");
        save_screen_capture(rejuv_enemy_capture, "rejuv_enemy");
        save_screen_capture(rejuv_enemy_tab_capture, "rejuv_enemy_tab");
        save_screen_capture(esc_icon_capture, "esc_icon");
    }
}

void GameStateTracker::save_screen_capture(const QImage& capture, const QString& name)
{
    if (!debug_dir.exists()) {
        debug_dir.mkpath(".");
    }
    auto filepath = QString(debug_dir.path() + "/%1_%2.png").arg(QString::number(number_of_scans), name);
    qDebug() << "Writing image file to: " << filepath;
    capture.save(filepath);
}

CVManager* GameStateTracker::get_cv_manager()
{
    return cv_manager;
}

OCRManager* GameStateTracker::get_ocr_manager()
{
    return ocr_manager;
}
