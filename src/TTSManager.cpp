#include "TTSManager.h"

TTSManager::TTSManager(SettingsManager* settings_manager, QObject* parent)
    : QObject(parent)
    , speech(new QTextToSpeech(this))
    , settings_manager(settings_manager)
{
    speech->setVolume(0.5);
    connect(settings_manager, &SettingsManager::settings_changed, this, &TTSManager::update_settings);

    update_settings();
}

void TTSManager::set_voice(int voice_num)
{
    speech->setVoice(speech->availableVoices()[voice_num]);
}

QStringList TTSManager::get_available_voices()
{
    QStringList voices;
    for (const auto& voice : speech->availableVoices()) {
        voices.append(voice.name());
    }
    return voices;
}

void TTSManager::set_volume(int volume)
{
    speech->setVolume(static_cast<double>(volume) / 100.0);
}

void TTSManager::say_test()
{
    speech->say("test");
}

void TTSManager::say(const QString& text)
{
    speech->say(text);
}

void TTSManager::handle_event(EventType type)
{
    const int lead_time = settings_manager->load_setting("timer/lead_time").toInt();
    switch(type) {
        case EventType::small_camps:
            speech->say("Small camps and breakables spawn in " + QString::number(lead_time) + " seconds");
            break;
        case EventType::medium_camps:
            speech->say("Medium camps spawn in " + QString::number(lead_time) + " seconds");
            break;
        case EventType::large_camps:
            speech->say("Sinners Sacrifice and large camps spawn in " + QString::number(lead_time) + " seconds");
            break;
        case EventType::mid_boss:
            speech->say("Mid Boss and breakables above it spawn in " + QString::number(lead_time) + " seconds");
            break;
        case EventType::runes:
            speech->say("Runes spawn in " + QString::number(lead_time) + " seconds");
            break;
        case EventType::urn:
            speech->say("Urn spawns in " + QString::number(lead_time) + " seconds");
            break;
        default:
            qDebug() << "Unknown Event Type";
            break;
    }
}

void TTSManager::update_settings()
{
    set_volume(settings_manager->load_setting("speech/volume").toInt());
    speech->setVoice(speech->availableVoices()[settings_manager->load_setting("speech/voice").toInt()]);
}
