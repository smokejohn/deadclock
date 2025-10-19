#include <QDebug>

#include "TimerController.h"
#include "SettingsManager.h"
#include <tesseract/baseapi.h>

TimerController::TimerController(SettingsManager* settings_manager, QObject* parent)
    : QObject(parent)
    , timer(new QTimer(this))
    , settings_manager(settings_manager)
    , clock_reader(new ClockReader(this))
{
    connect(timer, &QTimer::timeout, this, &TimerController::update_time);
    timer->setInterval(1000);

    connect(clock_reader, &ClockReader::time_read, this, &TimerController::update_time_external);
    connect(settings_manager, &SettingsManager::settings_changed, this, &TimerController::update_settings);

    enabled_events.set();
}

void TimerController::start()
{
    timer->start();
    clock_reader->start_reading();
    emit running_changed();
}

void TimerController::pause()
{
    timer->stop();
    clock_reader->stop_reading();
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

void TimerController::update_time_external(int minutes, int seconds)
{
    if (minutes == -1 || seconds == -1) {
        return;
    }

    int input_time = minutes * 60 + seconds;
    elapsed_seconds = input_time;
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
    const int lead_time = settings_manager->load_setting("timer/lead_time").toInt();

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
    enabled_events = std::bitset<6>(event_mask.toStdString());
}
