/**
 * @file OCRManager.h
 * @brief Wrapper class around tesseract OCR used to read text from ingame UI
 */

#pragma once

#include <QObject>
#include <QImage>
#include <QString>

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

#include <memory>

class OCRManager : public QObject
{
    Q_OBJECT

public:
    explicit OCRManager(QObject* parent = nullptr);
    ~OCRManager();

    QString detect_digits(QImage region, const std::string& region_name = "");

    int read_gametime(const QImage& region);
    std::pair<int, int> read_souls(const QImage& region_team, const QImage& region_enemy);

private:
    int parse_time_to_seconds(const QString& ocr_input);
    int parse_souls(const QString& ocr_input);
    QPair<int, int> parse_time(const QString& ocr_input);
    PIX* qimage_to_pix(const QImage& image);

    std::unique_ptr<tesseract::TessBaseAPI> ocr_engine;
};
