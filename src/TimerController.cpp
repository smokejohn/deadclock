#include <QDebug>

#include "TimerController.h"


TimerController::TimerController(QObject* parent)
    : QObject { parent }
    , timer(new QTimer(this))
    , speech(new QTextToSpeech(this))
{
    connect(timer, &QTimer::timeout, this, &TimerController::update_time);
    timer->setInterval(1000);

    auto voices = speech->availableVoices();

    // qDebug() << "Available voices";
    // for(const auto& voice : voices) {
        // qDebug() << voice.name();
    // }

    if (voices.size() > 1) {
        speech->setVoice(voices[0]);
    }

}

void TimerController::start() {
    timer->start();
    emit running_changed();
}

void TimerController::pause() {
    timer->stop();
    emit running_changed();
}

void TimerController::reset() {
    timer->stop();
    emit running_changed();
    elapsed_seconds = 0;

    emit time_changed();
}

bool TimerController::is_running() {
    return timer->isActive();
}

void TimerController::set_time() {
    set_time(last_set_minutes, last_set_seconds);
}

void TimerController::set_time(int minutes, int seconds) {
    last_set_minutes = minutes;
    last_set_seconds = seconds;
    elapsed_seconds = minutes * 60 + seconds;
    emit time_changed();
}

QString TimerController::display_time() const {
    unsigned int minutes = elapsed_seconds / 60;
    unsigned int seconds = elapsed_seconds % 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

void TimerController::update_time()
{
    manage_timers();
    elapsed_seconds++;
    emit time_changed();
}

void TimerController::say_test() {
    speech->say("test");
}

// QML Setters
void TimerController::set_speech_volume(int volume) {
    speech->setVolume(static_cast<double>(volume) / 100.0);
}

void TimerController::set_last_set_minutes(int minutes) {
    last_set_minutes = minutes;
}

void TimerController::set_last_set_seconds(int seconds) {
    last_set_seconds = seconds;
}

void TimerController::set_lead_time(int seconds) {
    lead_time = seconds;
}

void TimerController::manage_timers()
{
    const int spawn_time_breakables {120};
    const int spawn_time_small_camps {120};
    const int spawn_time_runes {300};
    const int spawn_time_medium_camps {360};
    const int spawn_time_large_camps {480};
    const int spawn_time_sinners {480};
    const int spawn_time_mid_boss {600};
    int spawn_time_urn {600};

    // One time events

    // Small camps and breakables
    if (elapsed_seconds == spawn_time_breakables - lead_time) {
        speech->say("Small camps and breakables spawn in " + QString::number(lead_time) + " seconds");
    }

    // Medium camps
    if (elapsed_seconds == spawn_time_medium_camps - lead_time) {
        speech->say("Medium camps spawn in " + QString::number(lead_time) + " seconds");
    }

    // Large camps and Sinners Sacrifice
    if (elapsed_seconds == spawn_time_large_camps - lead_time) {
        speech->say("Sinners Sacrifice and large camps spawn in " + QString::number(lead_time) + " seconds");
    }

    // Mid Boss and breakables in mid
    if (elapsed_seconds ==  spawn_time_mid_boss - lead_time) {
        speech->say("Mid Boss and breakables above it spawn in " + QString::number(lead_time) + " seconds");
    }


    // Repeating events
    // rune spawns every 5 minutes after 10 minutes
    if (elapsed_seconds > 600) {
        spawn_time_urn = 300;
    }

    // Runes
    // if (elapsed_seconds == spawn_time_runes * rune_spawn - lead_time) {
    if (elapsed_seconds % spawn_time_runes == spawn_time_runes - lead_time) {
        speech->say("Runes spawn in " + QString::number(lead_time) + " seconds");
    }


    // Urn
    // if (elapsed_seconds == spawn_time_urn * urn_spawn - lead_time) {
    if (elapsed_seconds % spawn_time_urn == spawn_time_urn - lead_time) {
        speech->say("Urn spawns in " + QString::number(lead_time) + " seconds");
    }

}
