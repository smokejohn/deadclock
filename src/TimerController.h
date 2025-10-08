#pragma once

#include <QObject>
#include <QTimer>
#include <QTextToSpeech>

class TimerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString display_time READ display_time NOTIFY time_changed)
    Q_PROPERTY(bool is_running READ is_running NOTIFY running_changed)

public:
    explicit TimerController(QObject* parent = nullptr);

    void set_time();

    Q_INVOKABLE void start();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void reset();
    Q_INVOKABLE void set_time(int minutes, int seconds);
    Q_INVOKABLE bool is_running();

    // TODO: move speech code out of this class
    Q_INVOKABLE void set_speech_volume(int volume);
    Q_INVOKABLE void say_test();

    Q_INVOKABLE void set_last_set_minutes(int minutes);
    Q_INVOKABLE void set_last_set_seconds(int seconds);

    Q_INVOKABLE void set_lead_time(int seconds);

    QString display_time() const;

signals:
    void time_changed();
    void running_changed();

public slots:
    void update_time_external(int minutes, int seconds);

private slots:
    void update_time();

private:
    void manage_timers();

    int last_set_minutes {0};
    int last_set_seconds {20};
    int lead_time {20};

    QTimer* timer;
    unsigned int elapsed_seconds {0};

    QTextToSpeech* speech;
};
