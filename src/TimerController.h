#pragma once

#include <QObject>
#include <QPointer>
#include <QTimer>

#include <bitset>
#include <qobject.h>

#include "SettingsManager.h"
#include "ClockReader.h"
#include "data/Common.h"

class TimerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString display_time READ display_time NOTIFY time_changed)
    Q_PROPERTY(bool is_running READ is_running NOTIFY running_changed)

public:
    explicit TimerController(SettingsManager* settings_manager, QObject* parent = nullptr);

    void set_time();

    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void reset();
    Q_INVOKABLE void set_time(int minutes, int seconds);
    Q_INVOKABLE bool is_running();

    Q_INVOKABLE void set_last_set_minutes(int minutes);
    Q_INVOKABLE void set_last_set_seconds(int seconds);

    QString display_time() const;

signals:
    void time_changed();
    void running_changed();
    void event_occured(EventType type);

public slots:
    void update_time_external(int minutes, int seconds);

private slots:
    void update_time();
    void update_settings();

private:
    void manage_timers();

    int last_set_minutes { 0 };
    int last_set_seconds { 20 };

    bool event_enabled(EventType type);

    QTimer* timer;
    ClockReader* clock_reader;
    unsigned int elapsed_seconds { 0 };
    QPointer<SettingsManager> settings_manager { nullptr };
    std::bitset<6> enabled_events;
};
