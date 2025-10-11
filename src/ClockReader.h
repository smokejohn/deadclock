#pragma once

#include <QObject>
#include <QPixmap>
#include <QRect>
#include <QString>
#include <QVariantMap>

#include <leptonica/allheaders.h>
#include <tesseract/baseapi.h>

#include <memory>

class ClockReader : public QObject
{
    Q_OBJECT

public:
    explicit ClockReader(QObject* parent = nullptr);
    ~ClockReader();

    Q_INVOKABLE void set_capture_region(int x, int y, int width, int height);
    Q_INVOKABLE QVariantMap get_capture_region();

    QString detect_digits();

    void capture_region();
    void save_to_disk(const QString& path);
    void start_reading();
    void stop_reading();

signals:
    void time_read(int minutes, int seconds);

private:
    int parse_text_to_seconds(const QString& ocr_input);
    QPair<int, int> parse_text_to_time(const QString& ocr_input);
    PIX* qimage_to_pix(const QImage& image);

    void read_clock();

    std::unique_ptr<tesseract::TessBaseAPI> ocr_engine;
    QTimer* read_timer;
    QRect capture_rect;
    QPixmap current_capture;
};
