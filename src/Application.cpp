#include "Application.h"
#include "NotificationManager.h"

#include <QIcon>
#include <QQmlContext>

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#include <dwmapi.h>
#endif

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

    minimap_overlay = engine.rootObjects().at(1)->findChild<QWindow*>("minimap_overlay");
    minimap_overlay->setFlag(Qt::WindowDoesNotAcceptFocus, true);
    minimap_overlay->setFlag(Qt::WindowTransparentForInput, true);

#ifdef PLATFORM_WINDOWS
    add_dwm_window_decoration();
#endif

    connect(timer_controller, &TimerController::event_occured, tts_manager, &TTSManager::handle_event);
    connect(
        timer_controller, &TimerController::event_occured, notification_manager, &NotificationManager::handle_event);
    connect(timer_controller, &TimerController::event_occured, this, &Application::handle_event);

    connect(input_manager, &InputManager::keybind_pressed, this, &Application::handle_keybind_pressed);

    toggle_overlay_locked(true);
}

Application::~Application()
{
}

void Application::handle_event(EventType type)
{
    if (type == EventType::minimap_drill) {
        emit highlight_minimap();
    }
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
        minimap_overlay->show();
    } else {
        timer_overlay->hide();
        notification_overlay->hide();
        minimap_overlay->hide();
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

#ifdef PLATFORM_WINDOWS
void Application::add_dwm_window_decoration()
{
    constexpr auto DWMWA_WINDOW_CORNER_PREFERENCE { 33 };
    constexpr auto DWMWCP_ROUND { 2 };
    constexpr auto DMMWA_USE_IMMERSIVE_DARK_MODE { 20 };

    HWND hwnd = reinterpret_cast<HWND>(main_window->winId());
    DWORD cornerPreference = DWMWCP_ROUND;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &cornerPreference, sizeof(cornerPreference));

    MARGINS margins = { 1, 1, 1, 1 };
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}
#endif
