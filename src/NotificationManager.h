/**
 * @file NotificationManager.h
 * @brief Manages display of on screen notifications on alerts
 */

#pragma once

#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QStringList>

#include "SettingsManager.h"
#include "data/Common.h"

class NotificationManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString message MEMBER message NOTIFY message_changed)
    Q_PROPERTY(int fade_time MEMBER fade_time NOTIFY fade_time_changed)
    Q_PROPERTY(int clear_time MEMBER clear_time NOTIFY clear_time_changed)

public:
    explicit NotificationManager(SettingsManager* settings_manager, QObject* parent = nullptr);

    void update_message(const QString& new_message);

signals:
    void message_changed();
    void fade_time_changed();
    void clear_time_changed();

public slots:
    void handle_event(EventType type);

private slots:
    void flush_aggregated_messages();
    void update_settings();

private:
    QString message;
    unsigned int fade_time {1000};
    unsigned int clear_time {5000};
    bool show_notifications {false};

    QStringList aggregated_events;
    QTimer* aggregation_timer;
    QPointer<SettingsManager> settings_manager { nullptr };
};
