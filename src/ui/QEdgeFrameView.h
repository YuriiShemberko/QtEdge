#ifndef QEDGEFRAMEVIEW_H
#define QEDGEFRAMEVIEW_H

#include <QWidget>
#include <QPaintEvent>
#include <core/QEdgeUtils.h>
#include <memory>
#include <mutex>

namespace Ui {
class QEdgeFrameView;
}

class QEdgeFrameView : public QWidget
{
    Q_OBJECT

public:
    explicit QEdgeFrameView(QWidget *parent = 0);
    ~QEdgeFrameView();
    void OnNewFrame( AVFrame* frame );

    virtual bool event( QEvent* event );
    virtual void paintEvent( QPaintEvent* ) override;

signals:
    void frameShown();
    void frameProcessed( AVFrame* frame );

protected:
    virtual void resizeEvent( QResizeEvent* ev ) override;

private:

    void ResetImage( QImage* image );

    class QUpdatePreviewEvent: public QEvent
    {
    public:
        static const QEvent::Type EventType = QEvent::Type( QEvent::User + 1 );
        QUpdatePreviewEvent() : QEvent( EventType ) {}
    };

    bool m_frame_shown;
    Ui::QEdgeFrameView *ui;
    AVFrame* m_current_frame;
    QImage* m_image;
    QImage m_curtain_img;
    std::mutex m_frame_mtx;
};

#endif // QEDGEFRAMEVIEW_H
