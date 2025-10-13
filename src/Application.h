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
    Q_INVOKABLE void toggle_notifications(bool show);

signals:
    void overlay_locked(bool locked);

private slots:
    void handle_keybind_pressed(KeyBind keybind);

private:
    QQmlApplicationEngine engine;
    QWindow* main_window;
    QWindow* timer_overlay;
    QWindow* notification_overlay;

    SettingsManager* settings_manager;
    TimerController* timer_controller;
    TTSManager* tts_manager;
    NotificationManager* notification_manager;
    InputManager* input_manager;
};
