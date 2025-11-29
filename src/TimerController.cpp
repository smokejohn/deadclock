#include <QDebug>

#include "SettingsManager.h"
#include "TimerController.h"
#include "gamestate/GameStateTracker.h"

TimerController::TimerController(SettingsManager* settings_manager, QObject* parent)
    : QObject(parent)
    , timer(new QTimer(this))
    , settings_manager(settings_manager)
    , gamestate_tracker(new GameStateTracker(this))
{
    connect(timer, &QTimer::timeout, this, &TimerController::update_time);
    timer->setInterval(1000);

    connect(gamestate_tracker, &GameStateTracker::time_read, this, &TimerController::update_time_external);
    connect(
        gamestate_tracker, &GameStateTracker::rejuv_buff_team_changed, this, &TimerController::update_rejuv_buff_team);
    connect(gamestate_tracker,
            &GameStateTracker::rejuv_buff_enemy_changed,
            this,
            &TimerController::update_rejuv_buff_enemy);
    connect(settings_manager, &SettingsManager::settings_changed, this, &TimerController::update_settings);

    enabled_events.set();
}

void TimerController::start()
{
    timer->start();
    gamestate_tracker->start_tracking();
    emit running_changed();
}

void TimerController::pause()
{
    timer->stop();
    gamestate_tracker->stop_tracking();
    emit running_changed();
}

void TimerController::reset()
{
    timer->stop();
    emit running_changed();
    elapsed_seconds = 0;

    emit time_changed();
}

bool TimerController::is_running()
{
    return timer->isActive();
}

void TimerController::set_time()
{
    set_time(last_set_minutes, last_set_seconds);
}

void TimerController::set_time(int minutes, int seconds)
{
    last_set_minutes = minutes;
    last_set_seconds = seconds;
    elapsed_seconds = minutes * 60 + seconds;

    rejuv_buff_enemy_time_left = 0;
    rejuv_buff_enemy_gained_time = 0;
    rejuv_buff_enemy_active = false;
    rejuv_buff_team_time_left = 0;
    rejuv_buff_team_gained_time = 0;
    rejuv_buff_team_active = false;
    gamestate_tracker->reset_gamestate();

    emit time_changed();
}

QString TimerController::display_time() const
{
    unsigned int minutes = elapsed_seconds / 60;
    unsigned int seconds = elapsed_seconds % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

void TimerController::update_time()
{
    manage_timers();
    elapsed_seconds++;
    emit time_changed();
}

void TimerController::update_time_external(int seconds)
{
    if (seconds == -1) {
        return;
    }

    elapsed_seconds = seconds;
    emit time_changed();
}

void TimerController::set_last_set_minutes(int minutes)
{
    last_set_minutes = minutes;
}

void TimerController::set_last_set_seconds(int seconds)
{
    last_set_seconds = seconds;
}

void TimerController::manage_timers()
{
    static const int spawn_time_breakables { 120 };
    static const int spawn_time_small_camps { 120 };
    static const int spawn_time_runes { 300 };
    static const int spawn_time_medium_camps { 360 };
    static const int spawn_time_large_camps { 480 };
    static const int spawn_time_sinners { 480 };
    static const int spawn_time_mid_boss { 600 };
    int spawn_time_urn { 600 };

    static const int rejuv_buff_duration { 240 };

    const int lead_time = settings_manager->load_setting("timer/lead_time").toInt();
    const int minimap_drill_interval = settings_manager->load_setting("timer/minimap_drill_interval").toInt();

    // One time events

    // Small camps and breakables
    if (event_enabled(EventType::small_camps) && elapsed_seconds == spawn_time_breakables - lead_time) {
        emit event_occured(EventType::small_camps);
    }

    // Medium camps
    if (event_enabled(EventType::medium_camps) && elapsed_seconds == spawn_time_medium_camps - lead_time) {
        emit event_occured(EventType::medium_camps);
    }

    // Large camps and Sinners Sacrifice
    if (event_enabled(EventType::large_camps) && elapsed_seconds == spawn_time_large_camps - lead_time) {
        emit event_occured(EventType::large_camps);
    }

    // Mid Boss and breakables in mid
    if (event_enabled(EventType::mid_boss) && elapsed_seconds == spawn_time_mid_boss - lead_time) {
        emit event_occured(EventType::mid_boss);
    }

    // Repeating events
    // Urn spawns every 5 minutes after 10 minutes
    if (elapsed_seconds > 600) {
        spawn_time_urn = 300;
    }

    // Runes
    if (event_enabled(EventType::runes) && elapsed_seconds % spawn_time_runes == spawn_time_runes - lead_time) {
        emit event_occured(EventType::runes);
    }

    // Urn
    if (event_enabled(EventType::urn) && elapsed_seconds % spawn_time_urn == spawn_time_urn - lead_time) {
        emit event_occured(EventType::urn);
    }

    // Drills
    if (event_enabled(EventType::minimap_drill) && elapsed_seconds != 0 &&
        elapsed_seconds % minimap_drill_interval == 0) {
        emit event_occured(EventType::minimap_drill);
    }

    // // Rejuvenator
    // if (rejuv_buff_team_active && elapsed_seconds == rejuv_buff_team_gained_time + rejuv_buff_duration - lead_time) {
    //     emit event_occured(EventType::rejuv_buff_team_timeout);
    // }
    //
    // if (rejuv_buff_enemy_active && elapsed_seconds == rejuv_buff_enemy_gained_time + rejuv_buff_duration - lead_time)
    // {
    //     emit event_occured(EventType::rejuv_buff_enemy_timeout);
    // }

    // TimerBars
    if (rejuv_buff_team_active) {
        rejuv_buff_team_time_left = rejuv_buff_team_gained_time + rejuv_buff_duration - elapsed_seconds;
        if (rejuv_buff_team_time_left == 0) {
            rejuv_buff_team_active = false;
        }
    } else {
        rejuv_buff_team_time_left = 0;
    }
    if (rejuv_buff_enemy_active) {
        rejuv_buff_enemy_time_left = rejuv_buff_enemy_gained_time + rejuv_buff_duration - elapsed_seconds;
        if (rejuv_buff_enemy_time_left == 0) {
            rejuv_buff_enemy_active = false;
        }
    } else {
        rejuv_buff_enemy_time_left = 0;
    }
}

bool TimerController::event_enabled(EventType type)
{
    return enabled_events.test(static_cast<std::size_t>(type));
}

void TimerController::update_settings()
{
    auto event_mask = settings_manager->load_setting("timer/enabled_events").toString();
    // reverse order as std::bitset indexes from the right to the left;
    std::reverse(event_mask.begin(), event_mask.end());
    enabled_events = std::bitset<7>(event_mask.toStdString());
}

void TimerController::update_rejuv_buff_team(bool active)
{
    rejuv_buff_team_active = active;
    if (rejuv_buff_team_active) {
        rejuv_buff_team_gained_time = elapsed_seconds;
    }
}

void TimerController::update_rejuv_buff_enemy(bool active)
{
    rejuv_buff_enemy_active = active;
    if (rejuv_buff_enemy_active) {
        rejuv_buff_enemy_gained_time = elapsed_seconds;
    }
}

GameStateTracker* TimerController::get_gamestate_tracker()
{
    return gamestate_tracker;
}
