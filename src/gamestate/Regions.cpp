#include "Regions.h"

#include <QDebug>

Regions::Regions(int screen_width, int screen_height, QObject* parent)
    : screen_width(screen_width)
    , screen_height(screen_height)
    , QObject(parent)
{
    qDebug() << "Doing capture region setup for resolution: " << screen_width << "x" << screen_height;

    // OCR Regions
    timer_region = qrect_from_percentages(TIMER_REGION_X, 0, TIMER_REGION_WIDTH, TIMER_REGION_HEIGHT);
    souls_team_region = qrect_from_percentages(SOULS_TEAM_REGION_X, 0, SOULS_REGION_WIDTH, SOULS_REGION_HEIGHT);
    souls_enemy_region = qrect_from_percentages(SOULS_ENEMY_REGION_X, 0, SOULS_REGION_WIDTH, SOULS_REGION_HEIGHT);

    // OPENCV Regions
    rejuv_team_region =
        qrect_from_percentages(REJUV_REGION_TEAM_X, REJUV_REGION_Y, REJUV_REGION_WIDTH, REJUV_REGION_HEIGHT);
    rejuv_team_tab_region =
        qrect_from_percentages(REJUV_REGION_TEAM_X, REJUV_REGION_TAB_Y, REJUV_REGION_WIDTH, REJUV_REGION_HEIGHT);
    rejuv_enemy_region =
        qrect_from_percentages(REJUV_REGION_ENEMY_X, REJUV_REGION_Y, REJUV_REGION_WIDTH, REJUV_REGION_HEIGHT);
    rejuv_enemy_tab_region =
        qrect_from_percentages(REJUV_REGION_ENEMY_X, REJUV_REGION_TAB_Y, REJUV_REGION_WIDTH, REJUV_REGION_HEIGHT);
    esc_icon_region =
        qrect_from_percentages(ESC_ICON_REGION_X, ESC_ICON_REGION_Y, ESC_ICON_REGION_WIDTH, ESC_ICON_REGION_HEIGHT);
}

QRect Regions::qrect_from_percentages(double x, double y, double width, double height) const
{
    int region_x = static_cast<int>(x * static_cast<double>(screen_width) + 0.5);
    int region_y = static_cast<int>(y * static_cast<double>(screen_height) + 0.5);
    int region_width = static_cast<int>(width * static_cast<double>(screen_width) + 0.5);
    int region_height = static_cast<int>(height * static_cast<double>(screen_height) + 0.5);

    return QRect(region_x, region_y, region_width, region_height);
}

double Regions::get_percentage_of(int part, int whole)
{
    return static_cast<double>(part) / static_cast<double>(whole);
}
