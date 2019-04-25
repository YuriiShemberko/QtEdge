#include "QDropDownVolumeSlider.h"
#include "ui_QDropDownVolumeSlider.h"

QDropDownVolumeSlider::QDropDownVolumeSlider(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QDropDownVolumeSlider)
{
    ui->setupUi(this);
}

QDropDownVolumeSlider::~QDropDownVolumeSlider()
{
    delete ui;
}
