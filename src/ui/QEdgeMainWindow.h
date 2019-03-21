#ifndef QEDGEMAINWINDOW_H
#define QEDGEMAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <core/CNullFrameProvider.h>

namespace Ui {
class QEdgeMainWindow;
}

class QEdgeMainWindow : public QMainWindow, public CNullFrameProviderClient
{
    Q_OBJECT

public:
    explicit QEdgeMainWindow(QWidget *parent = 0);
    void Init( IFrameProvider* provider );
    virtual void OnNewFrame( const QImage& image ) override;
    virtual QSize TargetSize() override;
    ~QEdgeMainWindow();

private slots:
    void OnPlayStopClicked();

private:
    Ui::QEdgeMainWindow *ui;
    IFrameProvider* m_provider;

};

#endif // QEDGEMAINWINDOW_H
