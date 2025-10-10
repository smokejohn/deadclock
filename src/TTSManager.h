#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextToSpeech>
#include <QPointer>

#include "data/Common.h"
#include "SettingsManager.h"

class TTSManager : public QObject
{
    Q_OBJECT

public:
    explicit TTSManager(SettingsManager* settings_manager, QObject* parent = nullptr);

    Q_INVOKABLE QStringList get_available_voices();
    Q_INVOKABLE void set_voice(int voice_num);
    Q_INVOKABLE void set_volume(int volume);
    Q_INVOKABLE void say_test();
    Q_INVOKABLE void say(const QString& text);

public slots:
    void handle_event(EventType type);
private slots:
    void update_settings();

private:
    QTextToSpeech* speech;
    QPointer<SettingsManager> settings_manager {nullptr};
};
