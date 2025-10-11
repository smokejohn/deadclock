#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQuickView>

#include "ClockReader.h"
#include "InputManager.h"
#include "SettingsManager.h"
#include "TTSManager.h"
#include "TimerController.h"

#include "data/Common.h"

class Application : public QObject
{
    Q_OBJECT

public:
    Application(QObject* parent = nullptr);
    ~Application();

    Q_INVOKABLE void toggle_overlay_visible(bool visible);
    Q_INVOKABLE void toggle_overlay_locked(bool lock);
    Q_INVOKABLE void toggle_clock_sync(bool sync);

signals:
    void overlay_locked(bool locked);

private slots:
    void handle_keybind_pressed(KeyBind keybind);

private:
    QQmlApplicationEngine engine;
    QWindow* main_window;
    QWindow* overlay_window;

    SettingsManager* settings_manager;
    ClockReader* clock_reader;
    TimerController* timer_controller;
    TTSManager* tts_manager;
    InputManager* input_manager;
};
