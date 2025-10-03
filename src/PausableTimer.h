#pragma once

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>

class PausableTimer : public QObject
{
    Q_OBJECT
public:
    explicit PausableTimer(QObject* parent = nullptr);

    void start(int duration_ms);
    void pause();
    void resume();
    void stop();

    void set_single_shot(bool single_shot);
    bool is_single_shot() const;

    bool is_running() const;
    bool is_paused() const;

signals:
    void timeout();

private:
    // The base timer we use for our pauseable timer
    QTimer* internal_timer;
    // A bridging timer for repeating timers that are paused and then resumed to handle the unfinished interval
    QTimer* bridge_timer;
    // A timer to hold the elapsed time (used when pausing and resuming)
    QElapsedTimer elapsed_timer;
    // The remaining duration when the timer is paused
    int remaining_duration {0};
    bool paused {false};
};
