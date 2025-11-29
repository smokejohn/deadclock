/**
 * @file OCRManager.h
 * @brief Wrapper class around tesseract OCR used to read text from ingame UI
 */

#pragma once

#include <QObject>
#include <QPixmap>
#include <QRect>
#include <QString>
#include <QVariantMap>

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

#include <memory>

class OCRManager : public QObject
{
    Q_OBJECT

public:
    explicit OCRManager(QObject* parent = nullptr);
    ~OCRManager();

    QString detect_digits(QRect region, const std::string& region_name = "");

    QPixmap capture_region(QRect region, const std::string& name = "");
    void save_to_disk(const QString& path);

    int read_gametime();
    std::pair<int, int> read_souls();

    int number_of_scans { 0 };
    bool debug_ocr { false };

private:
    int parse_time_to_seconds(const QString& ocr_input);
    int parse_souls(const QString& ocr_input);
    QPair<int, int> parse_time(const QString& ocr_input);
    PIX* qimage_to_pix(const QImage& image);

    std::unique_ptr<tesseract::TessBaseAPI> ocr_engine;
    QRect timer_region;
    QRect souls_team_region;
    QRect souls_enemy_region;
};
