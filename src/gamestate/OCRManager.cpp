#include "OCRManager.h"

#include <QBuffer>
#include <QIODevice>
#include <QRegularExpression>
#include <utility>


static const QRegularExpression clock_regex(R"((\d{1,2}):(\d{1,2}))");

OCRManager::OCRManager(QObject* parent)
    : QObject(parent)
    , ocr_engine(new tesseract::TessBaseAPI())
{
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

int OCRManager::read_gametime(const QImage& region)
{
    auto ocr_input = detect_digits(region, "timer");
    auto elapsed_gametime = parse_time_to_seconds(ocr_input);
    return elapsed_gametime;
}

std::pair<int, int> OCRManager::read_souls(const QImage& region_team, const QImage& region_enemy)
{
    auto ocr_input_team = detect_digits(region_team, "souls_team");
    auto ocr_input_enemy = detect_digits(region_enemy, "souls_enemy");

    int souls_team = parse_souls(ocr_input_team);
    int souls_enemy = parse_souls(ocr_input_enemy);

    return std::make_pair(souls_team, souls_enemy);
}

QString OCRManager::detect_digits(QImage region, const std::string& region_name)
{
    if (!ocr_engine) {
        qDebug() << "OCR engine not initialized";
        return "";
    }

    Pix* input_image = qimage_to_pix(region);
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

    std::vector<unsigned char> hex(data.size());
    std::memcpy(hex.data(), data.constData(), data.size());
    return pixReadMemBmp(hex.data(), data.size());
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
