/**
 * @file CVManager.h
 * @brief Wrapper around openCV which scans game UI for different info
 */

#pragma once

#include <QObject>
#include <QRect>

#include <opencv2/core/mat.hpp>

class CVManager : public QObject
{
    Q_OBJECT

public:
    explicit CVManager(QObject* parent = nullptr);
    ~CVManager();

    std::pair<int, int> detect_rejuv_buff();
    bool is_shop_open();

    /**
     * @brief find matching contours from template in target
     *
     * The input images should be bitmasks of well defined closed shapes for best results
     *
     * @param template_img the image to use as a template and generate the contours from
     * @param target_img the target to search the contours of the template in
     * @param result an optional out parameter that will draw the detected contours onto the target image
     *
     * @return a pair containing the number of matches in the target and the accuracy of the best match (0.0 - 1.0)
     */
    std::pair<int, double> find_matches_contour(const cv::Mat& template_img,
                                                const cv::Mat& target_img,
                                                cv::OutputArray result = cv::noArray());

    /**
     * @brief Creates a bitmask from an input image in the given color range
     *
     * @param input_img input image
     * @param result[out] the resulting bitmask
     * @param lower_bound 3 value scalar in HLS color space for lower bound
     * @param upper_bound 3 value scalar in HLS color space for upper bound
     */
    void mask_from_color_range(const cv::Mat& input_img,
                               cv::OutputArray result,
                               cv::Scalar lower_bound,
                               cv::Scalar upper_bound);

    /**
     * @brief Creates a bitmask from an input image by selecting the brigthest pixel
     *
     * @param input_img input image
     * @param result the resulting bitmask
     * @param threshold_mult a multiplier for the thresholding operations lower bound (the brightest detected pixel) allows scaling from black 0.0 to brightest pixel 1.0
     */
    void mask_from_brightest(const cv::Mat& input_img, cv::OutputArray result, double threshold_mult = 0.95);

    cv::Mat qimage_to_cv_mat(const QImage& input, bool clone_data = true);
    QImage cv_mat_to_qimage(const cv::Mat& input);

private:
    cv::Mat alpha_to_mask(const cv::Mat& input);

    QRect rejuv_team_capture_rect;
    QRect rejuv_enemy_capture_rect;
    QRect esc_icon_capture_rect;
};
