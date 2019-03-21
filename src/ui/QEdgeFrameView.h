#ifndef QEDGEFRAMEVIEW_H
#define QEDGEFRAMEVIEW_H

#include <QWidget>
#include <QPaintEvent>

namespace Ui {
class QEdgeFrameView;
}

class QEdgeFrameView : public QWidget
{
    Q_OBJECT

public:
    explicit QEdgeFrameView(QWidget *parent = 0);
    ~QEdgeFrameView();
    void UpdateImage( const QImage& image );

    virtual bool event( QEvent* event );
    virtual void paintEvent( QPaintEvent* ) override;

private:

    class QUpdatePreviewEvent: public QEvent
    {
    public:
        static const QEvent::Type EventType = QEvent::Type( QEvent::User + 1 );
        QUpdatePreviewEvent() : QEvent( EventType ) {}
    };

    Ui::QEdgeFrameView *ui;
    QImage m_image;
};

#endif // QEDGEFRAMEVIEW_H
