#include <QBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QSlider>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QtWidgets>

#include "LabeledSlider.h"
#include "gamestate/CVManager.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.resize(600, 400);
    window.setWindowTitle("OpenCV Algorithms test");

    QTabWidget tabs;
    QWidget contour_page;
    QWidget mask_from_brightest_page;

    QVBoxLayout main_container(&window);

    tabs.addTab(&contour_page, "Contour matching");
    tabs.addTab(&mask_from_brightest_page, "Mask from Brightest");

    main_container.addWidget(&tabs);

    QVBoxLayout contour_main_container(&contour_page);
    QVBoxLayout mask_main_container(&mask_from_brightest_page);

    // Contour Matching Tab
    QString template_image_path;
    QString search_image_path;

    QHBoxLayout slider_container;
    QVBoxLayout lowerb_sliders;
    QVBoxLayout upperb_sliders;

    LabeledSlider hue_lowb { "Hue Lower Bound", 0, 180 };
    LabeledSlider light_lowb { "Lightness Lower Bound", 0, 255 };
    LabeledSlider sat_lowb { "Saturation Lower Bound", 0, 255 };
    LabeledSlider hue_upperb { "Hue Upper Bound", 0, 180 };
    LabeledSlider light_upperb { "Lightness Upper Bound", 0, 255 };
    LabeledSlider sat_upperb { "Saturation Upper Bound", 0, 255 };

    QPushButton get_template_imagefile_button { "Pick template" };
    QPushButton get_search_imagefile_button { "Pick search image" };
    QPushButton find_matches_button { "find matches" };
    QLabel template_image_label;
    QLabel search_image_label;
    QLabel result_label;
    template_image_label.setScaledContents(true);
    search_image_label.setScaledContents(true);
    result_label.setScaledContents(true);
    template_image_label.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    search_image_label.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    result_label.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    QPixmap template_image;
    QPixmap search_image;
    QPixmap result_image;
    QLabel contour_match_label;

    CVManager cv_manager;

    lowerb_sliders.addWidget(&hue_lowb);
    lowerb_sliders.addWidget(&light_lowb);
    lowerb_sliders.addWidget(&sat_lowb);
    upperb_sliders.addWidget(&hue_upperb);
    upperb_sliders.addWidget(&light_upperb);
    upperb_sliders.addWidget(&sat_upperb);
    slider_container.addLayout(&lowerb_sliders);
    slider_container.addLayout(&upperb_sliders);
    contour_main_container.addLayout(&slider_container);
    contour_main_container.addWidget(&template_image_label);
    contour_main_container.addWidget(&get_template_imagefile_button);
    contour_main_container.addWidget(&search_image_label);
    contour_main_container.addWidget(&get_search_imagefile_button);
    contour_main_container.addWidget(&result_label);
    contour_main_container.addWidget(&find_matches_button);
    contour_main_container.addWidget(&contour_match_label);

    auto slider_handler = [&]() {
        cv::Mat template_img = cv_manager.qimage_to_cv_mat(template_image.toImage());
        cv::Mat result {};

        cv::Scalar lower_bound { static_cast<double>(hue_lowb.get_slider()->value()),
                                 static_cast<double>(light_lowb.get_slider()->value()),
                                 static_cast<double>(sat_lowb.get_slider()->value()) };
        cv::Scalar upper_bound {
            static_cast<double>(hue_upperb.get_slider()->value()),
            static_cast<double>(light_upperb.get_slider()->value()),
            static_cast<double>(sat_upperb.get_slider()->value()),
        };

        cv_manager.mask_from_color_range(template_img, result, lower_bound, upper_bound);
        auto mask_image = cv_manager.cv_mat_to_qimage(result);
        result_image = QPixmap::fromImage(mask_image);
        result_label.setPixmap(result_image);
    };

    QObject::connect(hue_lowb.get_slider(), &QSlider::sliderReleased, slider_handler);
    QObject::connect(light_lowb.get_slider(), &QSlider::sliderReleased, slider_handler);
    QObject::connect(sat_lowb.get_slider(), &QSlider::sliderReleased, slider_handler);
    QObject::connect(hue_upperb.get_slider(), &QSlider::sliderReleased, slider_handler);
    QObject::connect(light_upperb.get_slider(), &QSlider::sliderReleased, slider_handler);
    QObject::connect(sat_upperb.get_slider(), &QSlider::sliderReleased, slider_handler);

    QObject::connect(&get_template_imagefile_button, &QPushButton::clicked, [&]() {
        template_image_path = QFileDialog::getOpenFileName();
        template_image.load(template_image_path);
        template_image_label.setPixmap(template_image);
    });
    QObject::connect(&get_search_imagefile_button, &QPushButton::clicked, [&]() {
        search_image_path = QFileDialog::getOpenFileName();
        search_image.load(search_image_path);
        search_image_label.setPixmap(search_image);
    });

    QObject::connect(&find_matches_button, &QPushButton::clicked, [&]() {
        cv::Mat template_img = cv_manager.qimage_to_cv_mat(template_image.toImage());
        cv::Mat search_img = cv_manager.qimage_to_cv_mat(search_image.toImage());

        cv::Mat result {};
        auto [num_matches, match] = cv_manager.find_matches_contour(template_img, search_img, result);
        contour_match_label.setText("Number of matches: " + QString::number(num_matches) +
                                    " best match: " + QString::number(match));

        auto contour_image = cv_manager.cv_mat_to_qimage(result);
        result_label.setPixmap(QPixmap::fromImage(contour_image));
    });

    // Mask from brightest Tab
    QString mask_input_file_path;
    QPixmap mask_input_image;
    QLabel mask_input_image_label;
    QPixmap mask_result_image;
    QLabel mask_result_label;

    QPushButton get_input_file { "input file" };
    LabeledSlider mask_threshold_mult { "Threshold multiplier %", 0, 100 };

    QObject::connect(&get_input_file, &QPushButton::clicked, [&]() {
        mask_input_file_path = QFileDialog::getOpenFileName();
        mask_input_image.load(mask_input_file_path);
        mask_input_image_label.setPixmap(mask_input_image);
    });

    auto mask_slider_handler = [&]() {
        cv::Mat input_image = cv_manager.qimage_to_cv_mat(mask_input_image.toImage());
        cv::Mat result {};

        auto threshold_mult = static_cast<double>(mask_threshold_mult.get_slider()->value()) / 100;

        cv_manager.mask_from_brightest(input_image, result, threshold_mult);
        auto mask_image = cv_manager.cv_mat_to_qimage(result);
        mask_result_image = QPixmap::fromImage(mask_image);
        mask_result_label.setPixmap(mask_result_image);
    };

    QObject::connect(mask_threshold_mult.get_slider(), &QSlider::sliderReleased, mask_slider_handler);

    mask_main_container.addWidget(&mask_threshold_mult);
    mask_main_container.addWidget(&mask_input_image_label);
    mask_main_container.addWidget(&get_input_file);
    mask_main_container.addWidget(&mask_result_label);


    window.setLayout(&main_container);
    window.show();
    return app.exec();
}
