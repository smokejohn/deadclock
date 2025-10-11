#include "SettingsManager.h"


SettingsManager::SettingsManager(QObject* parent)
    : QObject(parent)
    , settings(new QSettings(QSettings::IniFormat, QSettings::UserScope, "FireAndForget", "Deadclock", this))
{}

SettingsManager::~SettingsManager()
{
}

void SettingsManager::save_setting(const QString& key, const QVariant& value)
{
    settings->setValue(key, value);
    emit settings_changed();
}

QVariant SettingsManager::load_setting(const QString& key, const QVariant& default_value)
{
    return settings->value(key, default_value);
}

QVariant SettingsManager::load_setting(const QString& key)
{
    auto variant = settings->value(key);
    if (variant.isValid()) {
        return variant;
    }

    auto default_value = defaults.find(key);
    if (default_value == defaults.end()) {
        qDebug() << "Cannot find default value for: " << key;
        return {};
    }

    return default_value->second;
}

void SettingsManager::set_to_defaults()
{
    for (const auto& [key, value] : defaults) {
        settings->setValue(key, value);
    }
    emit settings_changed();
}
