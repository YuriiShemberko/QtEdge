#ifndef QDROPDOWNVOLUMESLIDER_H
#define QDROPDOWNVOLUMESLIDER_H

#include <QWidget>
#include <QMainWindow>

namespace Ui {
class QDropDownVolumeSlider;
}

class QDropDownVolumeSlider : public QWidget
{
    Q_OBJECT

public:
    explicit QDropDownVolumeSlider( QWidget* dock_widget, int min, int max );
    void SetValue( int value );
    int GetValue();
    ~QDropDownVolumeSlider();

signals:
    void valueChanged( int value );

private slots:
    void OnValueChanged( int value );

protected:
    virtual bool eventFilter( QObject* watched, QEvent* event ) override;
    virtual bool event( QEvent* event ) override;

private:
    void OnParentLeave();

    Ui::QDropDownVolumeSlider *ui;
    QWidget* m_dock_widget;
};

#endif // QDROPDOWNVOLUMESLIDER_H
