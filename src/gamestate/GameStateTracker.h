/**
 * @file GameStateTracker.h
 * @brief Tracks deadlock gamestate with the help of OCR and ComputerVision
 */

#pragma once

#include <QDir>
#include <QObject>
#include <QImage>
#include <QScreen>
#include <QTimer>

#include "gamestate/CVManager.h"
#include "gamestate/OCRManager.h"
#include "gamestate/Regions.h"

struct GameState
{
    int gametime { 0 };
    int souls_team { 0 };
    int souls_enemy { 0 };
    int rejuv_buff_team { 0 };
    int rejuv_buff_enemy { 0 };
    bool shop_open { false };
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
    Q_INVOKABLE void set_debug_tracking(bool debug);

    CVManager* get_cv_manager();
    OCRManager* get_ocr_manager();

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
    QScreen* primary_screen;
    Regions* regions;

    bool debug_tracking { false };
    unsigned int number_of_scans { 0 };
    unsigned int scan_interval { 5000 };

    void update_screen_captures();
    QImage capture_region(QRect region);
    void save_screen_capture(const QImage& capture, const QString& name);

    // Regions of interest captures
    QImage timer_capture;
    QImage souls_team_capture;
    QImage souls_enemy_capture;

    QImage rejuv_team_capture;
    QImage rejuv_enemy_capture;
    QImage rejuv_team_tab_capture;
    QImage rejuv_enemy_tab_capture;
    QImage esc_icon_capture;

    QDir debug_dir { "./debug" };
};
