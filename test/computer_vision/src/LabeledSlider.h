#pragma once

#include <QSlider>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>

class LabeledSlider : public QWidget
{
public:
    LabeledSlider(const QString& text, int min, int max, QWidget* parent = nullptr);

    QLabel* get_label();
    QSlider* get_slider();

private:
    QString labeltext;
    QVBoxLayout* container;
    QLabel* label;
    QSlider* slider;
};
