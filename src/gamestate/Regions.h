/**
 * @file Regions.h
 * @brief Holds screen regions for gamestate capture
 */

#pragma once

#include <QObject>
#include <QRect>

// percentages of full frame measured on 2560x1440
// for positioning the capture area for ocr "independent" of screen resolution
// TODO: maybe read these values from json so if deadlock ui changes we can easily adapt

// The height of some region captures are increased by 11 pixels because when the player is dead
// and spectating teammates a solid colored 11 pixel bar is added to the top of the UI
// Ingame timer
constexpr double TIMER_REGION_X { 0.4805 }; // 1230 / 2560
constexpr double TIMER_REGION_WIDTH { 0.0390 }; // 100 / 2560
constexpr double TIMER_REGION_HEIGHT { 0.0250 }; // 25 + 11 / 1440

// Current Souls for team and enemy
constexpr double SOULS_TEAM_REGION_X { 0.4535 };  // 1161 / 2560
constexpr double SOULS_ENEMY_REGION_X { 0.5199 }; // 1331 / 2560
constexpr double SOULS_REGION_WIDTH { 0.0269 };   // 69 / 2560
constexpr double SOULS_REGION_HEIGHT { 0.0319 };  // 35 + 11 / 1440

// Rejuvenator buff icon regions for team and enemy
constexpr auto REJUV_REGION_TEAM_X { 0.4464 };  // 1143 / 2560
constexpr auto REJUV_REGION_ENEMY_X { 0.5140 }; // 1316 / 2560
constexpr auto REJUV_REGION_Y { 0.0278 };       // 40 / 1440
constexpr auto REJUV_REGION_WIDTH { 0.0390 };    // 100 / 2560
constexpr auto REJUV_REGION_HEIGHT { 0.0770 };    // 100 + 11 / 1440
constexpr auto REJUV_REGION_TAB_Y { 0.1187 };   // 171 / 1440

// ESC icon when shop or game menu is opened
constexpr auto ESC_ICON_REGION_X { 0.0074 };      // 19 / 2560
constexpr auto ESC_ICON_REGION_Y { 0.9611 };      // 1384 / 1440
constexpr auto ESC_ICON_REGION_WIDTH { 0.0289 };  // 72 / 2560
constexpr auto ESC_ICON_REGION_HEIGHT { 0.0263 }; // 38 / 1440

class Regions : public QObject
{
    Q_OBJECT

public:
    explicit Regions(int screen_width, int screen_height, QObject* parent = nullptr);

    QRect timer_region;
    QRect souls_team_region;
    QRect souls_enemy_region;

    QRect rejuv_team_region;
    QRect rejuv_enemy_region;
    QRect rejuv_team_tab_region;
    QRect rejuv_enemy_tab_region;
    QRect esc_icon_region;

private:
    [[nodiscard]] static double get_percentage_of(int part, int whole);
    [[nodiscard]] QRect qrect_from_percentages(double x, double y, double width, double height) const;

    int screen_width { 0 };
    int screen_height { 0 };
};
