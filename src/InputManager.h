#pragma once

#include <QObject>
#include <QPointer>

#include "Keyhook.h"
#include "SettingsManager.h"
#include "data/Common.h"

class InputManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool pause_keybind_active READ is_pause_keybind_active WRITE set_pause_keybind_active NOTIFY
                   pause_keybind_active_changed)
    Q_PROPERTY(int pause_key READ get_pause_key NOTIFY pause_keybind_changed)
    Q_PROPERTY(bool set_keybind_active READ is_set_keybind_active WRITE set_set_keybind_active NOTIFY
                   set_keybind_active_changed)
    Q_PROPERTY(int set_key READ get_set_key NOTIFY set_keybind_changed)

public:
    explicit InputManager(SettingsManager* settings_manager, QObject* parent = nullptr);

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
    void keybind_pressed(KeyBind keybind);

    void pause_keybind_active_changed();
    void pause_keybind_changed();

    void set_keybind_active_changed();
    void set_keybind_changed();

private slots:
    void key_pressed(unsigned int key);
    void update_settings();

private:
    void mouse_pressed(unsigned int button);
    void mouse_released(unsigned int button);

    Keyhook* keyhook;
    QPointer<SettingsManager> settings_manager { nullptr };
    int pause_key {};
    bool keybinding_pause_key { false };
    int set_key {};
    bool keybinding_set_key { false };
};
