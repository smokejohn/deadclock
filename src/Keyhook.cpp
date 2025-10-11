#include "Keyhook.h"

#include <QDebug>

#include <stdarg.h>
#include <uiohook.h>

Keyhook* Keyhook::instance = nullptr;

Keyhook::Keyhook(QObject* parent)
    : QThread(parent)
{
    instance = this;
}

Keyhook::~Keyhook()
{
    qDebug("Stopping system keyhook");
    int result = hook_stop();
    qDebug("Stopped system keyhook: %i", result);

    // ending Qthread
    quit();
    wait();

    instance = nullptr;
}

bool Keyhook::logger_proc(unsigned int level, const char* format, ...)
{
    bool status = false;

    va_list args;
    switch (level) {
        case LOG_LEVEL_INFO:
            va_start(args, format);
            status = vfprintf(stdout, format, args) >= 0;
            va_end(args);
            break;

        case LOG_LEVEL_WARN:
        case LOG_LEVEL_ERROR:
            va_start(args, format);
            status = vfprintf(stderr, format, args) >= 0;
            va_end(args);
            break;
    }

    return status;
}

void Keyhook::dispatch_wrapper(_uiohook_event* const event)
{
    if (instance) {
        instance->dispatch_proc(event);
    }
}

void Keyhook::dispatch_proc(uiohook_event* const event)
{
    switch (event->type) {
        case EVENT_KEY_PRESSED:
            emit key_pressed(event->data.keyboard.keycode);
            break;
        case EVENT_KEY_RELEASED:
            emit key_released(event->data.keyboard.keycode);
            break;
        case EVENT_MOUSE_PRESSED:
            emit mouse_pressed(event->data.mouse.button);
            break;
        case EVENT_MOUSE_RELEASED:
            emit mouse_released(event->data.mouse.button);
            break;
        default:
            break;
    }
}

void Keyhook::run()
{
    // Set the logger callback for uiohook output
    hook_set_logger_proc(&Keyhook::logger_proc);
    // Set the event callback for uiohook events
    hook_set_dispatch_proc(&Keyhook::dispatch_wrapper);

    // Start the hook and block.
    int status = hook_run();
    switch (status) {
        case UIOHOOK_SUCCESS:
            // Everything is ok.
            {
                printf("UIOHOOK_SUCCESS\n");
                break;
            }

        // System level errors.
        case UIOHOOK_ERROR_OUT_OF_MEMORY:
            logger_proc(LOG_LEVEL_ERROR, "Failed to allocate memory. (%#X)", status);
            break;

        // X11 specific errors.
        case UIOHOOK_ERROR_X_OPEN_DISPLAY:
            logger_proc(LOG_LEVEL_ERROR, "Failed to open X11 display. (%#X)", status);
            break;

        case UIOHOOK_ERROR_X_RECORD_NOT_FOUND:
            logger_proc(LOG_LEVEL_ERROR, "Unable to locate XRecord extension. (%#X)", status);
            break;

        case UIOHOOK_ERROR_X_RECORD_ALLOC_RANGE:
            logger_proc(LOG_LEVEL_ERROR, "Unable to allocate XRecord range. (%#X)", status);
            break;

        case UIOHOOK_ERROR_X_RECORD_CREATE_CONTEXT:
            logger_proc(LOG_LEVEL_ERROR, "Unable to allocate XRecord context. (%#X)", status);
            break;

        case UIOHOOK_ERROR_X_RECORD_ENABLE_CONTEXT:
            logger_proc(LOG_LEVEL_ERROR, "Failed to enable XRecord context. (%#X)", status);
            break;

        // Windows specific errors.
        case UIOHOOK_ERROR_SET_WINDOWS_HOOK_EX:
            logger_proc(LOG_LEVEL_ERROR, "Failed to register low level windows hook. (%#X)", status);
            break;

        // Darwin specific errors.
        case UIOHOOK_ERROR_AXAPI_DISABLED:
            logger_proc(LOG_LEVEL_ERROR, "Failed to enable access for assistive devices. (%#X)", status);
            break;

        case UIOHOOK_ERROR_CREATE_EVENT_PORT:
            logger_proc(LOG_LEVEL_ERROR, "Failed to create apple event port. (%#X)", status);
            break;

        case UIOHOOK_ERROR_CREATE_RUN_LOOP_SOURCE:
            logger_proc(LOG_LEVEL_ERROR, "Failed to create apple run loop source. (%#X)", status);
            break;

        case UIOHOOK_ERROR_GET_RUNLOOP:
            logger_proc(LOG_LEVEL_ERROR, "Failed to acquire apple run loop. (%#X)", status);
            break;

        case UIOHOOK_ERROR_CREATE_OBSERVER:
            logger_proc(LOG_LEVEL_ERROR, "Failed to create apple run loop observer. (%#X)", status);
            break;

        // Default error.
        case UIOHOOK_FAILURE:
        default:
            logger_proc(LOG_LEVEL_ERROR, "An unknown hook error occurred. (%#X)", status);
            break;
    }
}
