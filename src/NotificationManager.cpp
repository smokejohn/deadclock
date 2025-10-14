#include "NotificationManager.h"
#include "SettingsManager.h"

NotificationManager::NotificationManager(SettingsManager* settings_manager, QObject* parent)
    : QObject(parent)
    , settings_manager(settings_manager)
    , aggregation_timer(new QTimer(this))
{
    aggregation_timer->setSingleShot(true);
    aggregation_timer->setInterval(250);

    connect(aggregation_timer, &QTimer::timeout, this, &NotificationManager::flush_aggregated_messages);
}

void NotificationManager::update_message(const QString& new_message)
{
    message = new_message;
    qDebug() << "Notification Message changed to: " << message;
    emit message_changed();
}

void NotificationManager::handle_event(EventType type)
{
    qDebug() << "NotificationManager::handle_event called";
    if (!aggregation_timer->isActive()) {
        aggregation_timer->start();
    }

    QString message {};
    switch (type) {
        case EventType::small_camps:
            message = "Small camps and breakables spawn";
            break;
        case EventType::medium_camps:
            message = "Medium camps spawn";
            break;
        case EventType::large_camps:
            message = "Sinners Sacrifice and large camps spawn";
            break;
        case EventType::mid_boss:
            message = "Mid Boss spawns";
            break;
        case EventType::runes:
            message = "Runes spawn";
            break;
        case EventType::urn:
            message = "Urn spawns";
            break;
        default:
            qDebug() << "Unknown Event Type";
            break;
    }
    aggregated_events.append(message);
}

void NotificationManager::flush_aggregated_messages()
{
    const int lead_time = settings_manager->load_setting("timer/lead_time").toInt();

    QString final_message {"In " + QString::number(lead_time) + " seconds:"};
    for (const auto& message : aggregated_events) {
        final_message += "\n - " + message;
    }
    update_message(final_message);

    aggregated_events.clear();
}
