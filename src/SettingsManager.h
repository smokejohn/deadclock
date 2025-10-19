#pragma once

#include <QObject>
#include <QSettings>
#include <QVariant>

class SettingsManager : public QObject
{
    Q_OBJECT
public:
    SettingsManager(QObject* parent = nullptr);
    ~SettingsManager();

    Q_INVOKABLE void save_setting(const QString& key, const QVariant& value);
    Q_INVOKABLE QVariant load_setting(const QString& key, const QVariant& default_value);
    Q_INVOKABLE QVariant load_setting(const QString& key);
    void set_to_defaults();

signals:
    void settings_changed();

private:
    QSettings* settings;
    std::map<QString, QVariant> defaults { { "speech/volume", { 50 } },
                                           { "speech/voice", { 0 } },
                                           { "timer/lead_time", { 20 } },
                                           { "timer/enabled_events", { "111111" } },
                                           { "input/pause_key", { 26 } },
                                           { "input/set_key", { 27 } } };
};
