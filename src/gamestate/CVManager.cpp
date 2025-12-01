#include "CVManager.h"

#include <QGuiApplication>
#include <QImage>

#include <opencv2/core/mat.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>


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
    if (template_img.empty() || target_img.empty()) {
        qDebug() << "Template or target image don't contain data -> exiting";
        return std::make_pair(-1, 1);
    }

    cv::Mat template_processed {};
    cv::Mat target_processed {};

    // Closing holes
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(target_img, target_processed, cv::MORPH_CLOSE, kernel);

    // Blurring
    cv::Size blursize(3, 3);
    cv::GaussianBlur(template_img, template_processed, blursize, 0);
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
        }
    }

    if (!result.empty()) {
        result.assign(out);
    }
    return std::make_pair(num_matches, best_match);
}

void CVManager::mask_from_color_range(const cv::Mat& input_img,
                                      cv::OutputArray result,
                                      cv::Scalar lower_bound,
                                      cv::Scalar upper_bound)
{
    if (input_img.empty()) {
        qDebug() << "Input image empty -> exiting";
        return;
    }

    cv::Mat input_processed;
    cv::cvtColor(input_img, input_processed, cv::COLOR_BGR2HLS);

    cv::inRange(input_processed, lower_bound, upper_bound, input_processed);

    result.assign(input_processed);
}

bool CVManager::is_shop_open(const QImage& region)
{
    QImage esc_icon_template_image = QImage(":/images/opencv/dl_esc_icon.png");
    cv::Mat esc_icon_template = qimage_to_cv_mat(esc_icon_template_image);
    cv::Mat esc_icon_target = qimage_to_cv_mat(region);

    mask_from_brightest(esc_icon_template, esc_icon_template, 0.90);
    mask_from_brightest(esc_icon_target, esc_icon_target, 0.90);

    auto [matches, accuracy] = find_matches_contour(esc_icon_template, esc_icon_target);

    return matches != 0;
}

void CVManager::mask_from_brightest(const cv::Mat& input_img, cv::OutputArray result, double threshold_mult)
{
    if (input_img.empty()) {
        qDebug() << "Input image empty -> exiting";
        return;
    }

    cv::Mat processed_input {};
    cv::cvtColor(input_img, processed_input, cv::COLOR_BGR2GRAY);

    double min {};
    double max {};
    cv::minMaxLoc(processed_input, &min, &max);

    cv::threshold(processed_input, processed_input, max * threshold_mult, 255, cv::THRESH_BINARY);

    auto out = cv::Mat::zeros(input_img.size(), CV_8UC3);
    result.assign(processed_input);
}

std::pair<int, int> CVManager::detect_rejuv_buff(const QImage& region_team,
                                                 const QImage& region_team_tab,
                                                 const QImage& region_enemy,
                                                 const QImage& region_enemy_tab)
{
    QImage rejuv_icon_image = QImage(":/images/opencv/dl_rejuv_icon.png");
    cv::Mat target_region_team = qimage_to_cv_mat(region_team);
    cv::Mat target_region_enemy = qimage_to_cv_mat(region_enemy);
    cv::Mat target_region_team_tab = qimage_to_cv_mat(region_team_tab);
    cv::Mat target_region_enemy_tab = qimage_to_cv_mat(region_enemy_tab);
    cv::Mat rejuv_template = qimage_to_cv_mat(rejuv_icon_image);

    // Filtering by color range of rejuv icon to get mask
    // testing shows some good values are:
    cv::Scalar lower_bound(21, 80, 80);
    cv::Scalar upper_bound(25, 175, 255);
    mask_from_color_range(rejuv_template, rejuv_template, lower_bound, upper_bound);
    mask_from_color_range(target_region_team, target_region_team, lower_bound, upper_bound);
    mask_from_color_range(target_region_enemy, target_region_enemy, lower_bound, upper_bound);
    mask_from_color_range(target_region_team_tab, target_region_team_tab, lower_bound, upper_bound);
    mask_from_color_range(target_region_enemy_tab, target_region_enemy_tab, lower_bound, upper_bound);

    auto [matches_team, accuracy_team] = find_matches_contour(rejuv_template, target_region_team);
    auto [matches_enemy, accuracy_enemy] = find_matches_contour(rejuv_template, target_region_enemy);

    auto [matches_team_tab, accuracy_team_tab] = find_matches_contour(rejuv_template, target_region_team_tab);
    auto [matches_enemy_tab, accuracy_enemy_tab] = find_matches_contour(rejuv_template, target_region_enemy_tab);

    return std::make_pair(std::max(matches_team, matches_team_tab), std::max(matches_enemy, matches_enemy_tab));
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
