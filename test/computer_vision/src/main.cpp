#include <CVManager.h>
#include <QtWidgets>

#include <QFileDialog>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <qnamespace.h>

#include "LabeledSlider.h"


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.resize(600, 400);
    window.setWindowTitle("OpenCV Contour matching");

    QString template_image_path {};
    QString search_image_path {};

    QVBoxLayout main_container {};

    QHBoxLayout slider_container {};
    QVBoxLayout lowerb_sliders {};
    QVBoxLayout upperb_sliders {};

    LabeledSlider hue_lowb { "Hue Lower Bound", 0, 180 };
    LabeledSlider light_lowb { "Lightness Lower Bound", 0, 255 };
    LabeledSlider sat_lowb { "Saturation Lower Bound", 0, 255 };
    LabeledSlider hue_upperb { "Hue Upper Bound", 0, 180 };
    LabeledSlider light_upperb { "Lightness Upper Bound", 0, 255 };
    LabeledSlider sat_upperb { "Saturation Upper Bound", 0, 255 };

    QPushButton get_template_imagefile_button { "Pick template" };
    QPushButton get_search_imagefile_button { "Pick search image" };
    QPushButton find_matches_button { "find matches" };
    QLabel template_image_label {};
    QLabel search_image_label {};
    QLabel result_label {};
    template_image_label.setScaledContents(true);
    search_image_label.setScaledContents(true);
    result_label.setScaledContents(true);
    template_image_label.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    search_image_label.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    result_label.setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    QPixmap template_image {};
    QPixmap search_image {};
    QPixmap result_image {};
    QLabel contour_match_label {};

    CVManager cv_manager {};

    lowerb_sliders.addWidget(&hue_lowb);
    lowerb_sliders.addWidget(&light_lowb);
    lowerb_sliders.addWidget(&sat_lowb);
    upperb_sliders.addWidget(&hue_upperb);
    upperb_sliders.addWidget(&light_upperb);
    upperb_sliders.addWidget(&sat_upperb);
    slider_container.addLayout(&lowerb_sliders);
    slider_container.addLayout(&upperb_sliders);
    main_container.addLayout(&slider_container);
    main_container.addWidget(&template_image_label);
    main_container.addWidget(&get_template_imagefile_button);
    main_container.addWidget(&search_image_label);
    main_container.addWidget(&get_search_imagefile_button);
    main_container.addWidget(&result_label);
    main_container.addWidget(&find_matches_button);
    main_container.addWidget(&contour_match_label);

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
        double match = cv_manager.find_matches_contour(template_img, search_img, result);
        contour_match_label.setText(QString::number(match));

        auto contour_image = cv_manager.cv_mat_to_qimage(result);
        result_label.setPixmap(QPixmap::fromImage(contour_image));
    });


    window.setLayout(&main_container);
    window.show();
    return app.exec();
}
