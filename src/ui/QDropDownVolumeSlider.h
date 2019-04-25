#ifndef QDROPDOWNVOLUMESLIDER_H
#define QDROPDOWNVOLUMESLIDER_H

#include <QWidget>

namespace Ui {
class QDropDownVolumeSlider;
}

class QDropDownVolumeSlider : public QWidget
{
    Q_OBJECT

public:
    explicit QDropDownVolumeSlider(QWidget *parent = 0);
    ~QDropDownVolumeSlider();

private:
    Ui::QDropDownVolumeSlider *ui;
};

#endif // QDROPDOWNVOLUMESLIDER_H
