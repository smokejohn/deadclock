#include "Application.h"

#include <QQmlContext>
#include <QIcon>

#include "data/KeyMap.h"
#include <tesseract/baseapi.h>

Application::Application(QObject* parent)
    : QObject(parent)
    , settings_manager(new SettingsManager(this))
    , keyhook(new Keyhook(this))
    , clock_reader(new ClockReader(this))
    , timer_controller(new TimerController(settings_manager, this))
    , tts_manager(new TTSManager(settings_manager, this))
{
    keyhook->start();

    engine.rootContext()->setContextProperty("application", this);
    engine.rootContext()->setContextProperty("timer_controller", timer_controller);
    engine.rootContext()->setContextProperty("settings", settings_manager);
    engine.rootContext()->setContextProperty("tts", tts_manager);

    engine.load(QUrl(QStringLiteral("qrc:/ui/MainWindow.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/ui/OverlayWindow.qml")));

    main_window = qobject_cast<QWindow*>(engine.rootObjects().at(0));
    overlay_window = qobject_cast<QWindow*>(engine.rootObjects().at(1));

    toggle_keyhook_engaged(true);

    connect(clock_reader, &ClockReader::time_read, timer_controller, &TimerController::update_time_external);

    connect(timer_controller, &TimerController::event_occured, tts_manager, TTSManager::handle_event);
}

Application::~Application() {
    qDebug() << "Destroying window_controller";
}

void Application::toggle_overlay_locked(bool lock)
{
    overlay_window->setFlag(Qt::WindowDoesNotAcceptFocus, lock);
    overlay_window->setFlag(Qt::WindowTransparentForInput, lock);
    overlay_window->show();

    emit overlay_locked(lock);
}

void Application::toggle_overlay_visible(bool visible)
{
    if(visible) {
        overlay_window->show();
    } else {
        overlay_window->hide();
    }
}

void Application::toggle_keyhook_engaged(bool engaged) {
    if(engaged) {
        connect(keyhook, &Keyhook::key_pressed, this, &Application::key_pressed, Qt::QueuedConnection);
    } else {
        disconnect(keyhook, &Keyhook::key_pressed, this, &Application::key_pressed);
    }
}

void Application::toggle_clock_sync(bool sync) {
    if (sync) {
        clock_reader->start_reading();
    } else {
        clock_reader->stop_reading();
    }
}

void Application::key_pressed(unsigned int key)
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

void Application::key_released(unsigned int key)
{
    qDebug("Key Released");
}

void Application::toggle_pause_keybind_active() {
    set_pause_keybind_active(!keybinding_pause_key);
}

bool Application::is_pause_keybind_active() {
    return keybinding_pause_key;
}

int Application::get_pause_key() {
    return pause_key;
}

void Application::set_pause_keybind_active(bool active) {
    if(keybinding_pause_key == active)
        return;
    keybinding_pause_key = active;
    emit pause_keybind_active_changed();
}


void Application::toggle_set_keybind_active() {
    set_set_keybind_active(!keybinding_set_key);
}

bool Application::is_set_keybind_active() {
    return keybinding_set_key;
}

int Application::get_set_key() {
    return set_key;
}

void Application::set_set_keybind_active(bool active) {
    if(keybinding_set_key == active)
        return;
    keybinding_set_key = active;
    emit set_keybind_active_changed();
}

QString Application::get_key_name(int keycode) {
    return QString::fromStdString(KeyMap::get_key(keycode));
}
