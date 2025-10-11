#include "InputManager.h"
#include "data/KeyMap.h"

InputManager::InputManager(SettingsManager* settings_manager, QObject* parent)
    : QObject(parent)
    , keyhook(new Keyhook(this))
    , settings_manager(settings_manager)
{
    keyhook->start();
    connect(keyhook, &Keyhook::key_pressed, this, &InputManager::key_pressed, Qt::QueuedConnection);
    connect(settings_manager, &SettingsManager::settings_changed, this, &InputManager::update_settings);

    update_settings();
}

void InputManager::key_pressed(unsigned int key)
{
    if (keybinding_pause_key) {
        pause_key = key;
        settings_manager->save_setting("input/pause_key", pause_key);
        emit pause_keybind_changed();
        toggle_pause_keybind_active();
        return;
    }

    if (keybinding_set_key) {
        set_key = key;
        settings_manager->save_setting("input/set_key", set_key);
        emit set_keybind_changed();
        toggle_set_keybind_active();
        return;
    }

    if (key == pause_key) {
        emit keybind_pressed(KeyBind::pause);
    }
    if (key == set_key) {
        emit keybind_pressed(KeyBind::set);
    }
}

void InputManager::toggle_pause_keybind_active()
{
    set_pause_keybind_active(!keybinding_pause_key);
}

bool InputManager::is_pause_keybind_active()
{
    return keybinding_pause_key;
}

int InputManager::get_pause_key()
{
    return pause_key;
}

void InputManager::set_pause_keybind_active(bool active)
{
    if (keybinding_pause_key == active) {
        return;
    }
    keybinding_pause_key = active;
    emit pause_keybind_active_changed();
}


void InputManager::toggle_set_keybind_active()
{
    set_set_keybind_active(!keybinding_set_key);
}

bool InputManager::is_set_keybind_active()
{
    return keybinding_set_key;
}

int InputManager::get_set_key()
{
    return set_key;
}

void InputManager::set_set_keybind_active(bool active)
{
    if (keybinding_set_key == active) {
        return;
    }
    keybinding_set_key = active;
    emit set_keybind_active_changed();
}

QString InputManager::get_key_name(int keycode)
{
    return QString::fromStdString(KeyMap::get_key(keycode));
}

void InputManager::update_settings()
{
    pause_key = settings_manager->load_setting("input/pause_key").toInt();
    set_key = settings_manager->load_setting("input/set_key").toInt();
}
