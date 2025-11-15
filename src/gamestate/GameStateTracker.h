/**
 * @file GameStateTracker.h
 * @brief Tracks deadlock gamestate with the help of OCR and ComputerVision
 */

#pragma once

#include <QObject>
#include <QTimer>

#include "gamestate/CVManager.h"
#include "gamestate/OCRManager.h"

struct GameState
{
    int gametime { 0 };
    int souls_team { 0 };
    int souls_enemy { 0 };
    int rejuv_buff_team { 0 };
    int rejuv_buff_enemy { 0 };
    bool shop_open {false};
};

class GameStateTracker : public QObject
{
    Q_OBJECT

public:
    explicit GameStateTracker(QObject* parent = nullptr);

    void start_tracking();
    void stop_tracking();
    void set_scan_interval(unsigned int seconds = 5);
    void reset_gamestate();

signals:
    void time_read(int elapsed_seconds);
    void souls_read(int team, int enemy);
    void rejuvenator_buff_read(int team, int enemy);
    void rejuv_buff_team_changed(bool active);
    void rejuv_buff_enemy_changed(bool active);

private:
    void scan_gamestate();

private:
    GameState gamestate;
    CVManager* cv_manager;
    OCRManager* ocr_manager;
    QTimer* scan_timer;

    unsigned int scan_interval {5000};
};
