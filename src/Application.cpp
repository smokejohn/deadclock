#include "Application.h"
#include "NotificationManager.h"

#include <QIcon>
#include <QQmlContext>


Application::Application(QObject* parent)
    : QObject(parent)
    , settings_manager(new SettingsManager(this))
    , timer_controller(new TimerController(settings_manager, this))
    , tts_manager(new TTSManager(settings_manager, this))
    , notification_manager(new NotificationManager(settings_manager, this))
    , input_manager(new InputManager(settings_manager, this))
{

    engine.rootContext()->setContextProperty("application", this);
    engine.rootContext()->setContextProperty("timer_controller", timer_controller);
    engine.rootContext()->setContextProperty("settings", settings_manager);
    engine.rootContext()->setContextProperty("tts", tts_manager);
    engine.rootContext()->setContextProperty("notification", notification_manager);
    engine.rootContext()->setContextProperty("input", input_manager);

    engine.load(QUrl(QStringLiteral("qrc:/ui/MainWindow.qml")));
    engine.load(QUrl(QStringLiteral("qrc:/ui/OverlayWindow.qml")));

    main_window = qobject_cast<QWindow*>(engine.rootObjects().at(0));
    timer_overlay = engine.rootObjects().at(1)->findChild<QWindow*>("timer_overlay");
    notification_overlay = engine.rootObjects().at(1)->findChild<QWindow*>("notification_overlay");

    connect(timer_controller, &TimerController::event_occured, tts_manager, &TTSManager::handle_event);
    connect(input_manager, &InputManager::keybind_pressed, this, &Application::handle_keybind_pressed);

    toggle_overlay_locked(true);
    toggle_notifications(false);
}

Application::~Application()
{
}

void Application::toggle_overlay_locked(bool lock)
{
    timer_overlay->setFlag(Qt::WindowDoesNotAcceptFocus, lock);
    timer_overlay->setFlag(Qt::WindowTransparentForInput, lock);
    notification_overlay->setFlag(Qt::WindowDoesNotAcceptFocus, lock);
    notification_overlay->setFlag(Qt::WindowTransparentForInput, lock);

    if (timer_overlay->isVisible()) {
        timer_overlay->show();
    }

    if (notification_overlay->isVisible()) {
        notification_overlay->show();
    }

    emit overlay_locked(lock);
}

void Application::toggle_overlay_visible(bool visible)
{
    if (visible) {
        timer_overlay->show();
        notification_overlay->show();
    } else {
        timer_overlay->hide();
        notification_overlay->hide();
    }
}

void Application::toggle_notifications(bool show)
{
    if (show) {
        connect(timer_controller, &TimerController::event_occured, notification_manager, &NotificationManager::handle_event);
    } else {
        disconnect(timer_controller, &TimerController::event_occured, notification_manager, &NotificationManager::handle_event);
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
