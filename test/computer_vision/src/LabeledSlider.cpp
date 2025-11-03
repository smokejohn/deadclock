#include "LabeledSlider.h"
#include <qboxlayout.h>

LabeledSlider::LabeledSlider(const QString& text, int min, int max, QWidget* parent)
    : labeltext(text)
    , container(new QVBoxLayout(this))
    , label(new QLabel(this))
    , slider(new QSlider(Qt::Horizontal, this))
{
    label->setText(labeltext + ": " + QString::number(min));
    slider->setRange(min, max);
    container->addWidget(label);
    container->addWidget(slider);

    connect(slider, &QSlider::valueChanged, this, [this](int value){
        label->setText(labeltext + ": " + QString::number(value));
    });
}

QLabel* LabeledSlider::get_label() {
    return label;
}

QSlider* LabeledSlider::get_slider() {
    return slider;
}
