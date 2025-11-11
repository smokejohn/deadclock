#include "GameStateTracker.h"

GameStateTracker::GameStateTracker(QObject* parent)
    : QObject(parent)
    , cv_manager(new CVManager(this))
    , ocr_manager(new OCRManager(this))
    , scan_timer(new QTimer(this))
{
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

void GameStateTracker::scan_gamestate()
{
    gamestate.shop_open = cv_manager->is_shop_open();

    if (gamestate.shop_open) {
        qDebug() << "Shop or Menu is open, cannot update gamestate";
        return;
    }

    gamestate.gametime = ocr_manager->read_gametime();
    emit time_read(gamestate.gametime);

    auto [souls_team, souls_enemy] = ocr_manager->read_souls();
    gamestate.souls_team = souls_team;
    gamestate.souls_enemy = souls_enemy;

    auto [rejuv_team, rejuv_enemy] = cv_manager->detect_rejuv_buff();
    if (gamestate.rejuv_buff_team = 0 && rejuv_team > 0) {
        emit rejuv_buff_gained();
    }
    if (gamestate.rejuv_buff_team > 0 && rejuv_team == 0) {
        emit rejuv_buff_lost();
    }

    gamestate.rejuv_buff_team = rejuv_team;
    gamestate.rejuv_buff_enemy = rejuv_enemy;

    qDebug() << "Gamestate:";
    qDebug() << "gametime: " << gamestate.gametime;
    qDebug() << "shop open: " << gamestate.shop_open;
    qDebug() << "souls: " << gamestate.souls_team << " | " << gamestate.souls_enemy;
    qDebug() << "rejuv: " << gamestate.rejuv_buff_team << " | " << gamestate.rejuv_buff_enemy;
}
