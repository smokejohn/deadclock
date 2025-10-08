#include "ClockReader.h"

#include <QScreen>
#include <QPixmap>
#include <QBuffer>
#include <QIODevice>
#include <QRegularExpression>
#include <QTimer>


// The DeadLock timer is at the top center of the screen
// These values are percentages of the current screen size
// for positioning the capture area for ocr "independent" of screen resolution
// TODO: read these values from json so if deadlock ui changes we can easily adapt
constexpr double X_POS_PERCENTAGE {0.4805};
constexpr double WIDTH_PERCENTAGE {0.039};
constexpr double HEIGHT_PERCENTAGE {0.0174};

static const QRegularExpression clock_regex(R"((\d{1,2}):(\d{1,2}))");

ClockReader::ClockReader(QObject* parent)
    : QObject(parent)
    , ocr_engine(new tesseract::TessBaseAPI())
    , read_timer(new QTimer(this))
{
    // initialize capture region
    // TODO: make this available as function
    QScreen* screen = QGuiApplication::primaryScreen();
    int x_pos = static_cast<int>(X_POS_PERCENTAGE * static_cast<double>(screen->geometry().width()) + 0.5);
    int width = static_cast<int>(WIDTH_PERCENTAGE * static_cast<double>(screen->geometry().width()) + 0.5);
    int height = static_cast<int>(HEIGHT_PERCENTAGE * static_cast<double>(screen->geometry().height()) + 0.5);
    set_capture_region(x_pos, 0, width, height);


    if(ocr_engine->Init("./", "eng"/*, tesseract::OEM_LSTM_ONLY*/)) {
        qDebug() << "Could not initialize tesseract";
        ocr_engine.reset();
        return;
    }

    ocr_engine->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
    ocr_engine->SetVariable("tessedit_char_whitelist", "0123456789:");


    connect(read_timer, &QTimer::timeout, this, &ClockReader::read_clock);
    read_timer->setInterval(5000);
}

ClockReader::~ClockReader() {
    if(ocr_engine) {
        ocr_engine->End();
    }
}

void ClockReader::start_reading() {
    read_timer->start();
}

void ClockReader::stop_reading() {
    read_timer->stop();
}

void ClockReader::read_clock(){
    auto ocr_input = detect_digits();
    auto [minutes, seconds] = parse_text_to_time(ocr_input);
    emit time_read(minutes, seconds);
}

QString ClockReader::detect_digits() {
    if (!ocr_engine) {
        qDebug() << "OCR engine not initialized";
        return "";
    }

    capture_region();

    Pix* input_image = qimage_to_pix(current_capture.toImage());
    if (!input_image) {
        qDebug() << "Could not open image file";
        return "";
    }

    ocr_engine->SetImage(input_image);
    QString detected_text = ocr_engine->GetUTF8Text();
    qDebug() << "Detected text: " << detected_text;

    ocr_engine->Clear();
    pixDestroy(&input_image);

    return detected_text;
}

Pix* ClockReader::qimage_to_pix(const QImage& image) {
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "BMP");
    // const std::size_t count = data.size();
    std::vector<unsigned char> hex (data.size());
    std::memcpy(hex.data(), data.constData(), data.size());
    return pixReadMemBmp(hex.data(), data.size());
}

void ClockReader::capture_region() {
    QScreen* screen = QGuiApplication::primaryScreen();

    if(!screen) {
        qDebug() << "Cannot get screen";
        return;
    }

    qDebug() << "Capturing region: " << capture_rect;
    current_capture = screen->grabWindow(0, capture_rect.left(), capture_rect.top(), capture_rect.width(), capture_rect.height());
}

void ClockReader::set_capture_region(int x, int y, int width, int height) {
    capture_rect.setRect(x, y, width, height);
    qDebug() << "Capture region changed: " << capture_rect;
}

QVariantMap ClockReader::get_capture_region() {
    QVariantMap rect_extents;
    rect_extents["x"] = capture_rect.left();
    rect_extents["y"] = capture_rect.top();
    rect_extents["width"] = capture_rect.width();
    rect_extents["height"] = capture_rect.height();
    return rect_extents;
}

void ClockReader::save_to_disk(const QString& path) {
    current_capture.save(path);
}

int ClockReader::parse_text_to_seconds(const QString& ocr_input) {
    if (ocr_input.isEmpty()) {
        qDebug() << "ocr input is empty";
        return 0;
    }

    QRegularExpressionMatch match = clock_regex.match(ocr_input);
    if (match.hasMatch()) {
        int minutes = match.captured(1).toInt();
        int seconds = match.captured(2).toInt();
        return minutes * 60 + seconds;
    }

    qDebug() << "Could not get a regex match";
    return 0;
}

QPair<int, int> ClockReader::parse_text_to_time(const QString& ocr_input) {
    if (ocr_input.isEmpty()) {
        qDebug() << "ocr input is empty";
        return qMakePair(-1, -1);
    }

    QRegularExpressionMatch match = clock_regex.match(ocr_input);
    if (match.hasMatch()) {
        int minutes = match.captured(1).toInt();
        int seconds = match.captured(2).toInt();
        return qMakePair(minutes, seconds);
    }

    qDebug() << "Could not get a regex match";
    return qMakePair(-1, -1);
}
