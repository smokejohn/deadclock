#include "OCRManager.h"

#include <QBuffer>
#include <QIODevice>
#include <QPixmap>
#include <QRegularExpression>
#include <QScreen>
#include <QDir>
#include <utility>


// The DeadLock timer is at the top center of the screen
// These values are percentages of the current screen size
// for positioning the capture area for ocr "independent" of screen resolution
// TODO: maybe read these values from json so if deadlock ui changes we can easily adapt
constexpr double TIMER_X_POS_PERCENTAGE { 0.4805 };
constexpr double TIMER_WIDTH_PERCENTAGE { 0.039 };
constexpr double TIMER_HEIGHT_PERCENTAGE { 0.0174 };

constexpr double SOULS_TEAM_X_POS_PERCENTAGE { 0.4535 };  // 1161 / 2560
constexpr double SOULS_ENEMY_X_POS_PERCENTAGE { 0.5199 }; // 1331 / 2560
constexpr double SOULS_WIDTH_PERCENTAGE { 0.0269 };       // 69 / 2560
constexpr double SOULS_HEIGHT_PERCENTAGE { 0.0243 };      // 35 / 1440

static const QRegularExpression clock_regex(R"((\d{1,2}):(\d{1,2}))");

OCRManager::OCRManager(QObject* parent)
    : QObject(parent)
    , ocr_engine(new tesseract::TessBaseAPI())
{
    // initialize capture regions
    QScreen* screen = QGuiApplication::primaryScreen();
    timer_region.setRect(
        static_cast<int>(TIMER_X_POS_PERCENTAGE * static_cast<double>(screen->geometry().width()) + 0.5),
        0,
        static_cast<int>(TIMER_WIDTH_PERCENTAGE * static_cast<double>(screen->geometry().width()) + 0.5),
        static_cast<int>(TIMER_HEIGHT_PERCENTAGE * static_cast<double>(screen->geometry().height()) + 0.5));
    souls_team_region.setRect(
        static_cast<int>(SOULS_TEAM_X_POS_PERCENTAGE * static_cast<double>(screen->geometry().width()) + 0.5),
        0,
        static_cast<int>(SOULS_WIDTH_PERCENTAGE * static_cast<double>(screen->geometry().width()) + 0.5),
        static_cast<int>(SOULS_HEIGHT_PERCENTAGE * static_cast<double>(screen->geometry().height()) + 0.5));
    souls_enemy_region.setRect(
        static_cast<int>(SOULS_ENEMY_X_POS_PERCENTAGE * static_cast<double>(screen->geometry().width()) + 0.5),
        0,
        static_cast<int>(SOULS_WIDTH_PERCENTAGE * static_cast<double>(screen->geometry().width()) + 0.5),
        static_cast<int>(SOULS_HEIGHT_PERCENTAGE * static_cast<double>(screen->geometry().height()) + 0.5));


    if (ocr_engine->Init("./", "eng")) {
        qDebug() << "Could not initialize tesseract";
        ocr_engine.reset();
        return;
    }

    ocr_engine->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
    ocr_engine->SetVariable("tessedit_char_whitelist", "0123456789:k");
}

OCRManager::~OCRManager()
{
    if (ocr_engine) {
        ocr_engine->End();
    }
}

int OCRManager::read_gametime()
{
    auto ocr_input = detect_digits(timer_region, "timer");
    auto elapsed_gametime = parse_time_to_seconds(ocr_input);
    return elapsed_gametime;
}

std::pair<int, int> OCRManager::read_souls()
{
    auto ocr_input_team = detect_digits(souls_team_region, "souls_team");
    auto ocr_input_enemy = detect_digits(souls_enemy_region, "souls_enemy");

    int souls_team = parse_souls(ocr_input_team);
    int souls_enemy = parse_souls(ocr_input_enemy);

    return std::make_pair(souls_team, souls_enemy);
}

QString OCRManager::detect_digits(QRect region, const std::string& region_name)
{
    if (!ocr_engine) {
        qDebug() << "OCR engine not initialized";
        return "";
    }

    auto target_region = capture_region(region, region_name);

    Pix* input_image = qimage_to_pix(target_region.toImage());
    if (!input_image) {
        qDebug() << "Could not open image file";
        return "";
    }

    ocr_engine->SetImage(input_image);
    QString detected_text = ocr_engine->GetUTF8Text();

    ocr_engine->Clear();
    pixDestroy(&input_image);

    return detected_text;
}

Pix* OCRManager::qimage_to_pix(const QImage& image)
{
    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "BMP");
    // const std::size_t count = data.size();
    std::vector<unsigned char> hex(data.size());
    std::memcpy(hex.data(), data.constData(), data.size());
    return pixReadMemBmp(hex.data(), data.size());
}

QPixmap OCRManager::capture_region(QRect region, const std::string& name)
{
    QScreen* screen = QGuiApplication::primaryScreen();

    if (!screen) {
        qDebug() << "Could not get primary screen";
        return QPixmap();
    }

    // qDebug() << "Capturing region: " << region;
    auto capture = screen->grabWindow(0, region.left(), region.top(), region.width(), region.height());

    if (debug_ocr) {
        QDir dir("./ocr_captures");
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        auto filepath = QString("./ocr_captures/%1_%2.png").arg(QString::number(number_of_scans), QString::fromStdString(name));
        qDebug() << "Writing capture to: " << filepath;
        capture.save(filepath);
    }
    return capture;
}

int OCRManager::parse_souls(const QString& ocr_input)
{
    if (ocr_input.isEmpty()) {
        qDebug() << "ocr input is empty";
        return -1;
    }

    // Do some primitive parsing
    auto souls_text = ocr_input.trimmed();

    int multiplier { 1 };
    if (souls_text.endsWith('k', Qt::CaseInsensitive)) {
        multiplier = 1000;
        souls_text.chop(1);
    }

    bool conversion_success { false };
    int souls = souls_text.toInt(&conversion_success);

    if (conversion_success) {
        return souls * multiplier;
    } else {
        qDebug() << "Could not parse souls to number from source: " << ocr_input;
        return -1;
    }
}

int OCRManager::parse_time_to_seconds(const QString& ocr_input)
{
    if (ocr_input.isEmpty()) {
        qDebug() << "ocr input is empty";
        return -1;
    }

    QRegularExpressionMatch match = clock_regex.match(ocr_input);
    if (match.hasMatch()) {
        int minutes = match.captured(1).toInt();
        int seconds = match.captured(2).toInt();
        return minutes * 60 + seconds;
    }

    qDebug() << "Could not get a regex match on source: " << ocr_input;
    return -1;
}

QPair<int, int> OCRManager::parse_time(const QString& ocr_input)
{
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

    qDebug() << "Could not get a regex match on source: " << ocr_input;
    return qMakePair(-1, -1);
}
