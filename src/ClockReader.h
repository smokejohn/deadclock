#pragma once

#include <QObject>
#include <tesseract/baseapi.h>

class ClockReader : public QObject
{
    Q_OBJECT
public:
    explicit ClockReader(QObject* parent = nullptr);
    ~ClockReader();

    void set_capture_region(int x, int y, int width, int height);
    void capture_region();
    void detect_digits();

signals:

private:
    Pix* qimage_to_pix(const QImage& image);
};
