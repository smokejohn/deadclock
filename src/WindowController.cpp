#include "WindowController.h"

#include <QQmlContext>
#include <QIcon>

#include "data/KeyMap.h"

WindowController::WindowController(QObject* parent)
    : QObject(parent)
    , keyhook(new Keyhook(this))
    , timer_controller(new TimerController(this))
{
    keyhook->start();

    engine.rootContext()->setContextProperty("window_controller", this);
    engine.rootContext()->setContextProperty("keyhook", keyhook);
    engine.rootContext()->setContextProperty("timer_controller", timer_controller);


    engine.load(QUrl(QStringLiteral("qrc:/ui/MainWindow.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/ui/OverlayWindow.qml")));

    main_window = qobject_cast<QWindow*>(engine.rootObjects().at(0));
    overlay_window = qobject_cast<QWindow*>(engine.rootObjects().at(1));

    toggle_keyhook_engaged(true);
}

void WindowController::toggle_overlay_raised(bool raised)
{
    overlay_window->setFlag(Qt::WindowStaysOnTopHint, raised);
    overlay_window->show();
}

void WindowController::toggle_overlay_locked(bool lock)
{
    overlay_window->setFlag(Qt::WindowDoesNotAcceptFocus, lock);
    overlay_window->setFlag(Qt::WindowTransparentForInput, lock);
    overlay_window->show();

    emit overlay_locked(lock);
}

void WindowController::toggle_overlay_visible(bool visible)
{
    if(visible) {
        overlay_window->show();
    } else {
        overlay_window->hide();
    }
}

void WindowController::toggle_keyhook_engaged(bool engaged) {
    if(engaged) {
        connect(keyhook, &Keyhook::key_pressed, this, &WindowController::key_pressed, Qt::QueuedConnection);
    } else {
        disconnect(keyhook, &Keyhook::key_pressed, this, &WindowController::key_pressed);
    }
}


void WindowController::key_pressed(unsigned int key)
{
    if(keybinding_pause_key) {
        pause_key = key;
        emit pause_keybind_changed();
        return;
    }

    if(keybinding_set_key) {
        set_key = key;
        emit set_keybind_changed();
        return;
    }

    if (key == pause_key) {
        if (timer_controller->is_running()) {
            timer_controller->pause();
            return;
        }
        timer_controller->start();
    }
    if (key == set_key) {
        timer_controller->set_time();
    }
}

void WindowController::key_released(unsigned int key)
{
    qDebug("Key Released");
}

void WindowController::toggle_pause_keybind_active() {
    set_pause_keybind_active(!keybinding_pause_key);
}

bool WindowController::is_pause_keybind_active() {
    return keybinding_pause_key;
}

int WindowController::get_pause_key() {
    return pause_key;
}

void WindowController::set_pause_keybind_active(bool active) {
    if(keybinding_pause_key == active)
        return;
    keybinding_pause_key = active;
    emit pause_keybind_active_changed();
}


void WindowController::toggle_set_keybind_active() {
    set_set_keybind_active(!keybinding_set_key);
}

bool WindowController::is_set_keybind_active() {
    return keybinding_set_key;
}

int WindowController::get_set_key() {
    return set_key;
}

void WindowController::set_set_keybind_active(bool active) {
    if(keybinding_set_key == active)
        return;
    keybinding_set_key = active;
    emit set_keybind_active_changed();
}

QString WindowController::get_key_name(int keycode) {
    return QString::fromStdString(KeyMap::get_key(keycode));
}
