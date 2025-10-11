#include "Application.h"

#include <QIcon>
#include <QQmlContext>

#include <tesseract/baseapi.h>

Application::Application(QObject* parent)
    : QObject(parent)
    , settings_manager(new SettingsManager(this))
    , clock_reader(new ClockReader(this))
    , timer_controller(new TimerController(settings_manager, this))
    , tts_manager(new TTSManager(settings_manager, this))
    , input_manager(new InputManager(settings_manager, this))
{

    engine.rootContext()->setContextProperty("application", this);
    engine.rootContext()->setContextProperty("timer_controller", timer_controller);
    engine.rootContext()->setContextProperty("settings", settings_manager);
    engine.rootContext()->setContextProperty("tts", tts_manager);
    engine.rootContext()->setContextProperty("input", input_manager);

    engine.load(QUrl(QStringLiteral("qrc:/ui/MainWindow.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/ui/OverlayWindow.qml")));

    main_window = qobject_cast<QWindow*>(engine.rootObjects().at(0));
    overlay_window = qobject_cast<QWindow*>(engine.rootObjects().at(1));

    connect(clock_reader, &ClockReader::time_read, timer_controller, &TimerController::update_time_external);
    connect(timer_controller, &TimerController::event_occured, tts_manager, &TTSManager::handle_event);
    connect(input_manager, &InputManager::keybind_pressed, this, &Application::handle_keybind_pressed);
}

Application::~Application()
{
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
    if (visible) {
        overlay_window->show();
    } else {
        overlay_window->hide();
    }
}

void Application::toggle_clock_sync(bool sync)
{
    if (sync) {
        clock_reader->start_reading();
    } else {
        clock_reader->stop_reading();
    }
}

void Application::handle_keybind_pressed(KeyBind keybind)
{
    if (keybind == KeyBind::pause) {
        if (timer_controller->is_running()) {
            timer_controller->pause();
            return;
        }
        timer_controller->start();
        return;
    }
    if (keybind == KeyBind::set) {
        timer_controller->set_time();
        return;
    }
}
