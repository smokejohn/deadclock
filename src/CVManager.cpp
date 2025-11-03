#include "CVManager.h"

#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

// percentages of full frame measured on 2560x1440
constexpr auto REJUV_SCAN_REGION_TEAM_X { 0.4464 };  // 1143 / 2560
constexpr auto REJUV_SCAN_REGION_ENEMY_X { 0.5140 }; // 1316 / 2560
constexpr auto REJUV_SCAN_REGION_Y { 0.0278 };       // 40 / 1440
constexpr auto REJUV_SCAN_REGION_SIZE { 0.0390 };    // 100 / 2560

CVManager::CVManager(QObject* parent)
    : QObject(parent)
{
}

CVManager::~CVManager()
{
}

std::pair<int, double> CVManager::find_matches_contour(const cv::Mat& template_img,
                                                       const cv::Mat& target_img,
                                                       cv::OutputArray result)
{
    cv::Mat template_processed {};
    cv::Mat target_processed {};

    // Filtering by color range to get mask
    // testing shows some good values are:
    cv::Scalar lower_bound(21, 80, 80);
    cv::Scalar upper_bound(25, 175, 255);
    mask_from_color_range(template_img, template_processed, lower_bound, upper_bound);
    mask_from_color_range(target_img, target_processed, lower_bound, upper_bound);

    // Blurring
    cv::Size blursize(3, 3);
    cv::GaussianBlur(template_processed, template_processed, blursize, 0);
    cv::GaussianBlur(target_processed, target_processed, blursize, 0);

    // Edge detect
    cv::Canny(template_processed, template_processed, 100, 200);
    cv::Canny(target_processed, target_processed, 100, 200);

    // Get Contours
    std::vector<std::vector<cv::Point>> template_contours {};
    cv::findContours(template_processed, template_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<std::vector<cv::Point>> target_contours {};
    cv::findContours(target_processed, target_contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);


    // Match Contours
    cv::Mat out { cv::Mat::zeros(target_img.size(), CV_8UC3) };
    double threshold { 0.2 };
    double best_match { 1.0 };
    int num_matches { 0 };
    for (size_t i = 0; i < target_contours.size(); ++i) {
        double match_value = cv::matchShapes(template_contours[0], target_contours[i], cv::CONTOURS_MATCH_I1, 0);
        if (match_value < threshold) {
            cv::drawContours(out, target_contours, i, cv::Scalar(0, 255, 0), 2);
            ++num_matches;
            if (match_value < best_match) {
                best_match = match_value;
            }
        } else {
            // cv::drawContours(out, target_contours, i, cv::Scalar(127, 127, 127), 1);
        }
    }

    result.assign(out);
    return std::make_pair(num_matches, best_match);
}

void CVManager::mask_from_color_range(const cv::Mat& input_img,
                                      cv::OutputArray result,
                                      cv::Scalar lower_bound,
                                      cv::Scalar upper_bound)
{
    cv::Mat input_processed;
    cv::cvtColor(input_img, input_processed, cv::COLOR_BGR2HLS);

    cv::inRange(input_processed, lower_bound, upper_bound, input_processed);

    result.assign(input_processed);
}

void CVManager::detect_rejuv_buff()
{
    QScreen* screen = QGuiApplication::primaryScreen();

    if (!screen) {
        qDebug() << "Could not get primary screen";
        return;
    }

    int x_pos_team = static_cast<int>(REJUV_SCAN_REGION_TEAM_X * static_cast<double>(screen->geometry().width()) + 0.5);
    int x_pos_enemy =
        static_cast<int>(REJUV_SCAN_REGION_ENEMY_X * static_cast<double>(screen->geometry().width()) + 0.5);
    int y_pos = static_cast<int>(REJUV_SCAN_REGION_Y * static_cast<double>(screen->geometry().width()) + 0.5);
    int size = static_cast<int>(REJUV_SCAN_REGION_SIZE * static_cast<double>(screen->geometry().height()) + 0.5);

    rejuv_team_capture_rect.setRect(x_pos_team, y_pos, size, size);
    rejuv_enemy_capture_rect.setRect(x_pos_enemy, y_pos, size, size);

    QPixmap rejuv_team_roi = screen->grabWindow(0,
                                                rejuv_team_capture_rect.left(),
                                                rejuv_team_capture_rect.top(),
                                                rejuv_team_capture_rect.width(),
                                                rejuv_team_capture_rect.height());

    QPixmap rejuv_enemy_roi = screen->grabWindow(0,
                                                 rejuv_team_capture_rect.left(),
                                                 rejuv_team_capture_rect.top(),
                                                 rejuv_team_capture_rect.width(),
                                                 rejuv_team_capture_rect.height());

    QImage rejuv_icon_image = QImage("qrc:/resources/images/rejuv_icon.png");
    cv::Mat target_region_team = qimage_to_cv_mat(rejuv_team_roi.toImage());
    cv::Mat target_region_enemy = qimage_to_cv_mat(rejuv_enemy_roi.toImage());
    cv::Mat rejuv_template = qimage_to_cv_mat(rejuv_icon_image);

    // Filtering by color range of rejuv icon to get mask
    // testing shows some good values are:
    cv::Scalar lower_bound(21, 80, 80);
    cv::Scalar upper_bound(25, 175, 255);
    mask_from_color_range(rejuv_template, rejuv_template, lower_bound, upper_bound);
    mask_from_color_range(target_region_team, target_region_team, lower_bound, upper_bound);
    mask_from_color_range(target_region_enemy, target_region_enemy, lower_bound, upper_bound);

    auto matches_team = find_matches_contour(rejuv_template, target_region_team);
    auto matches_enemy = find_matches_contour(rejuv_template, target_region_enemy);
}

cv::Mat CVManager::qimage_to_cv_mat(const QImage& input, bool clone_data)
{
    switch (input.format()) {
        // 8-bit, 4 channel - RGBA
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied: {
            cv::Mat mat(input.height(),
                        input.width(),
                        CV_8UC4,
                        const_cast<uchar*>(input.bits()),
                        static_cast<size_t>(input.bytesPerLine()));
            return (clone_data ? mat.clone() : mat);
        }
        // 8-bit, 3 channel - RGB 32
        case QImage::Format_RGB32: {
            cv::Mat mat(input.height(),
                        input.width(),
                        CV_8UC4,
                        const_cast<uchar*>(input.bits()),
                        static_cast<size_t>(input.bytesPerLine()));
            cv::Mat mat_no_alpha;
            cv::cvtColor(mat, mat_no_alpha, cv::COLOR_BGRA2BGR); // drops the all-white alpha channel
            return mat_no_alpha;
        }
        // 8-bit, 3 channel - RGB
        case QImage::Format_RGB888: {
            QImage swapped = input.rgbSwapped();
            return cv::Mat(swapped.height(),
                           swapped.width(),
                           CV_8UC3,
                           const_cast<uchar*>(swapped.bits()),
                           static_cast<size_t>(swapped.bytesPerLine()))
                .clone();
        }
        // 8-bit, 1 channel - Grayscale
        case QImage::Format_Indexed8: {
            cv::Mat mat(input.height(),
                        input.width(),
                        CV_8UC1,
                        const_cast<uchar*>(input.bits()),
                        static_cast<size_t>(input.bytesPerLine()));
            return (clone_data ? mat.clone() : mat);
        }
        default:
            qWarning() << "QImage format not handled: " << input.format();
            break;
    }

    return cv::Mat();
}

QImage CVManager::cv_mat_to_qimage(const cv::Mat& input)
{
    switch (input.type()) {
        // 8-bit, 4 channel - RGBA
        case CV_8UC4: {
            QImage image(input.data, input.cols, input.rows, static_cast<int>(input.step), QImage::Format_ARGB32);
            return image;
        }
        // 8-bit, 3 channel - RGB
        case CV_8UC3: {
            QImage image(input.data, input.cols, input.rows, static_cast<int>(input.step), QImage::Format_RGB888);
            return image.rgbSwapped();
        }
        // 8-bit, 1 channel - Grayscale
        case CV_8UC1: {
            QImage image(input.data, input.cols, input.rows, static_cast<int>(input.step), QImage::Format_Grayscale8);
            return image;
        }
        default:
            qWarning() << "cv::Mat image type not handled: " << input.type();
            break;
    }
    return QImage();
}

cv::Mat CVManager::alpha_to_mask(const cv::Mat& input)
{
    if (input.channels() < 4) {
        qWarning() << "Input doesn't have an alpha channel";
        return cv::Mat {};
    }

    cv::Mat mask_output {};
    std::vector<cv::Mat> input_channels {};

    cv::split(input, input_channels);
    mask_output = input_channels.back().clone();

    return mask_output;
}
