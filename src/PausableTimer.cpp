#include "PausableTimer.h"

#include <QDebug>

PausableTimer::PausableTimer(QObject* parent)
    : QObject(parent)
    , internal_timer(new QTimer(this))
    , bridge_timer(new QTimer(this))
{
    connect(internal_timer, &QTimer::timeout, this, &PausableTimer::timeout);

    connect(this, &PausableTimer::timeout, this, [](){
        qDebug() << "Timer has finished";
    });


    // needed for repeating timers
    bridge_timer->setSingleShot(true);
    connect(bridge_timer, &QTimer::timeout, this, [this]() {
        emit timeout();
        internal_timer->start();
    });


}


void PausableTimer::start(int duration_ms)
{
    qDebug() << "Starting timer with " << duration_ms << "ms duration";

    stop();
    paused = false;

    remaining_duration = duration_ms;

    elapsed_timer.start();
    internal_timer->start(duration_ms);
}

void PausableTimer::pause()
{
    qDebug() << "Pausing timer";

    if ((!internal_timer->isActive() && !bridge_timer->isActive()) || paused) {
        return;
    }

    if (internal_timer->isSingleShot()) {
        remaining_duration = remaining_duration - elapsed_timer.elapsed();
    } else {
        remaining_duration = remaining_duration - (elapsed_timer.elapsed() % internal_timer->interval());
    }
    qDebug() << "Timer has " << remaining_duration << "ms left";
    internal_timer->stop();
    bridge_timer->stop();

    paused = true;
}

void PausableTimer::resume()
{
    qDebug() << "Resuming timer with remaining duration: " << remaining_duration;

    if (!paused || remaining_duration <= 0) {
        return;
    }

    paused = false;

    if (internal_timer->isSingleShot()) {
        internal_timer->start(remaining_duration);
    } else {
        // repeating timer we need to continue the current interval with the remaining time
        // then repeat with the originally set delay/timeout
        bridge_timer->start(remaining_duration);
    }

    elapsed_timer.start();
}

void PausableTimer::stop()
{
    qDebug() << "Stopping timer";

    internal_timer->stop();
    bridge_timer->stop();

    paused = false;
    remaining_duration = 0;
}

bool PausableTimer::is_running() const
{
    return internal_timer->isActive();
}

void PausableTimer::set_single_shot(bool single_shot)
{
    internal_timer->setSingleShot(single_shot);
}

bool PausableTimer::is_single_shot() const
{
    return internal_timer->isSingleShot();
}

bool PausableTimer::is_paused() const {
    return paused;
}
