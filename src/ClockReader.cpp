#include "ClockReader.h"

#include <QScreen>
#include <QPixmap>

#include <leptonica/allheaders.h>

ClockReader::ClockReader(QObject* parent) {
    QScreen* screen = QGuiApplication::primaryScreen();

    if(!screen) {
        qDebug() << "cannot get screen";
        return;
    }

    qDebug() << "Capturing region";
    QPixmap capture_region = screen->grabWindow(0, 0, 0, 200, 200);

    capture_region.save("capture_region.png");
}

ClockReader::~ClockReader() {
}

Pix* ClockReader::qimage_to_pix(const QImage& image) {
    QImage converted = image.convertToFormat(QImage::Format_RGB888);
    int width = converted.width();
    int height = converted.height();
    int bytesPerLine = converted.bytesPerLine();

    // Create Leptonica Pix from raw data
    Pix* pix = pixCreate(width, height, 24);
    l_uint32* pixData = pixGetData(pix);
    int pixStride = pixGetWpl(pix) * 4;

    for (int y = 0; y < height; ++y) {
        const uchar* line = converted.scanLine(y);
        for (int x = 0; x < width; ++x) {
            int r = line[x * 3];
            int g = line[x * 3 + 1];
            int b = line[x * 3 + 2];
            pixData[y * pixGetWpl(pix) + x] = (r << 24) | (g << 16) | (b << 8);
        }
    }

    return pix;

    // Usage
    // QPixmap pixmap = ...; // Your captured pixmap
    // QImage image = pixmap.toImage();
    // Pix* pix = QImageToPix(image);
    //
    // tesseract::TessBaseAPI ocr;
    // ocr.Init(nullptr, "eng");
    // ocr.SetImage(pix);
    // char* text = ocr.GetUTF8Text();
    // qDebug() << "Detected text:" << text;
    //
    // delete[] text;
    // ocr.End();
    // pixDestroy(&pix);

}
