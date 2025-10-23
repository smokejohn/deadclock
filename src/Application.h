#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQuickView>

#include "InputManager.h"
#include "NotificationManager.h"
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

signals:
    void overlay_locked(bool locked);
    void highlight_minimap();

private slots:
    void handle_keybind_pressed(KeyBind keybind);
    void handle_event(EventType type);

private:
    QQmlApplicationEngine engine;
    QWindow* main_window;
    QWindow* timer_overlay;
    QWindow* notification_overlay;
    QWindow* minimap_overlay;

#ifdef PLATFORM_WINDOWS
    void add_dwm_window_decoration();
#endif

    SettingsManager* settings_manager;
    TimerController* timer_controller;
    TTSManager* tts_manager;
    NotificationManager* notification_manager;
    InputManager* input_manager;
};
