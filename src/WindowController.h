#pragma once

#include <QObject>
#include <QQmlApplicationEngine>
#include <QQuickView>

#include "Keyhook.h"
#include "TimerController.h"
#include "ClockReader.h"

class WindowController : public QObject
{
    Q_OBJECT
    Q_PROPERTY (bool pause_keybind_active READ is_pause_keybind_active WRITE set_pause_keybind_active NOTIFY pause_keybind_active_changed)
    Q_PROPERTY (int pause_key READ get_pause_key NOTIFY pause_keybind_changed)
    Q_PROPERTY (bool set_keybind_active READ is_set_keybind_active WRITE set_set_keybind_active NOTIFY set_keybind_active_changed)
    Q_PROPERTY (int set_key READ get_set_key NOTIFY set_keybind_changed)

public:
    WindowController(QObject* parent = nullptr);

    Q_INVOKABLE void toggle_overlay_visible(bool visible);
    Q_INVOKABLE void toggle_overlay_raised(bool raised);
    Q_INVOKABLE void toggle_overlay_locked(bool lock);
    Q_INVOKABLE void toggle_keyhook_engaged(bool engaged);

    Q_INVOKABLE QString get_key_name(int keycode);

    Q_INVOKABLE void toggle_pause_keybind_active();
    bool is_pause_keybind_active();
    int get_pause_key();
    void set_pause_keybind_active(bool active);

    Q_INVOKABLE void toggle_set_keybind_active();
    bool is_set_keybind_active();
    int get_set_key();
    void set_set_keybind_active(bool active);


signals:
    void overlay_locked(bool locked);

    void pause_keybind_active_changed();
    void pause_keybind_changed();

    void set_keybind_active_changed();
    void set_keybind_changed();

private:
    int pause_key {};
    bool keybinding_pause_key {false};

    int set_key {};
    bool keybinding_set_key {false};

    void mouse_pressed(unsigned int button);
    void mouse_released(unsigned int button);
    void key_pressed(unsigned int key);
    void key_released(unsigned int key);

    QQmlApplicationEngine engine;
    QWindow* main_window;
    QWindow* overlay_window;

    Keyhook* keyhook;
    TimerController* timer_controller;
    ClockReader* clock_reader;
};
