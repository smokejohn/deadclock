/**
 * @file Keyhook.h
 * @brief Wrapper around uiohook library to listen for keypresses globally without having application focus
 */

#pragma once

#include <uiohook.h>

#include <QThread>

class Keyhook : public QThread
{
    Q_OBJECT

public:
    explicit Keyhook(QObject* parent = nullptr);
    ~Keyhook();

signals:
    void mouse_pressed(unsigned int button);
    void mouse_released(unsigned int button);
    void key_pressed(unsigned int key);
    void key_released(unsigned int key);

protected:
    void run() override;

private:
    static bool logger_proc(unsigned int level, const char* format, ...);

    void dispatch_proc(_uiohook_event* const event);
    static void dispatch_wrapper(_uiohook_event* const event);

    static Keyhook* instance;
};
