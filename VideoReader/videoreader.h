#ifndef VIDEOREADER_H
#define VIDEOREADER_H

#include <QMainWindow>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QFileDialog>
#include <QtMultimedia/QAudioOutput>
#include <QtCore>
#include <QTime>
#include <iostream>
#include <QScreen>
#include <QMouseEvent>
#include <QCursor>

QT_BEGIN_NAMESPACE
namespace Ui { class VideoReader; }
QT_END_NAMESPACE

class VideoReader : public QMainWindow
{
    Q_OBJECT

public:
    VideoReader(QWidget *parent = nullptr);
    ~VideoReader();

private slots:
    void on_close_clicked();
    void on_option_clicked();
    void on_Play_clicked();
    void on_pause_clicked();
    void on_stop_clicked();
    void detectTimeVariation(qint64 duration);
    void positionChanged(qint64 progress);



    void on_volumeAjust_valueChanged(int value);

    void on_videoProgress_valueChanged(int value);

    void on_videoProgress_sliderPressed();

    void on_videoProgress_sliderReleased();


protected:
    enum ResizeRegion {
        None,
        Left,
        Right,
        Top,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    const int BORDER_WIDTH = 8;

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            currentRegion = detectResizeRegion(event->pos());
            if (currentRegion != None) {
                isResizing = true;
                resizeStartPos = event->globalPosition().toPoint();
                originalGeometry = geometry();
            } else if (titleBar->geometry().contains(event->pos())) {
                isDragging = true;
                dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            }
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        QPoint globalPos = event->globalPosition().toPoint();

        if (isDragging) {
            move(globalPos - dragPosition);
        } else if (isResizing) {
            handleResizing(globalPos);
        } else {
            updateCursor(event->pos());
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            isDragging = false;
            isResizing = false;
            currentRegion = None;
        }
    }

    void handleResizing(const QPoint &globalPos) {
        QRect newGeom = originalGeometry;
        int dx = globalPos.x() - resizeStartPos.x();
        int dy = globalPos.y() - resizeStartPos.y();

        switch (currentRegion) {
            case Left:
                newGeom.setLeft(newGeom.left() + dx);
                break;
            case Right:
                newGeom.setRight(newGeom.right() + dx);
                break;
            case Top:
                newGeom.setTop(newGeom.top() + dy);
                break;
            case Bottom:
                newGeom.setBottom(newGeom.bottom() + dy);
                break;
            case TopLeft:
                newGeom.setTopLeft(newGeom.topLeft() + QPoint(dx, dy));
                break;
            case TopRight:
                newGeom.setTopRight(newGeom.topRight() + QPoint(dx, dy));
                break;
            case BottomLeft:
                newGeom.setBottomLeft(newGeom.bottomLeft() + QPoint(dx, dy));
                break;
            case BottomRight:
                newGeom.setBottomRight(newGeom.bottomRight() + QPoint(dx, dy));
                break;
            default:
                break;
        }

        if (newGeom.width() >= minimumWidth() && newGeom.height() >= minimumHeight())
            setGeometry(newGeom);
    }

    ResizeRegion detectResizeRegion(const QPoint &pos) {
        QRect r = rect();
        bool onLeft = pos.x() <= BORDER_WIDTH;
        bool onRight = pos.x() >= r.width() - BORDER_WIDTH;
        bool onTop = pos.y() <= BORDER_WIDTH;
        bool onBottom = pos.y() >= r.height() - BORDER_WIDTH;

        if (onTop && onLeft) return TopLeft;
        if (onTop && onRight) return TopRight;
        if (onBottom && onLeft) return BottomLeft;
        if (onBottom && onRight) return BottomRight;
        if (onLeft) return Left;
        if (onRight) return Right;
        if (onTop) return Top;
        if (onBottom) return Bottom;

        return None;
    }

    void updateCursor(const QPoint &pos) {
        switch (detectResizeRegion(pos)) {
            case Top:
            case Bottom:
                setCursor(Qt::SizeVerCursor);
                break;
            case Left:
            case Right:
                setCursor(Qt::SizeHorCursor);
                break;
            case TopLeft:
            case BottomRight:
                setCursor(Qt::SizeFDiagCursor);
                break;
            case TopRight:
            case BottomLeft:
                setCursor(Qt::SizeBDiagCursor);
                break;
            default:
                setCursor(Qt::ArrowCursor);
                break;
        }
    }

private:
    Ui::VideoReader *ui;
    QString         *filename;
    QMediaPlayer    *vPlayer;
    QVideoWidget    *vOutput;
    QAudioOutput    *audioOut;
    float           volume;
    qint64          Mduration;
    void    updateTime(qint64 newValue);
    QWidget* titleBar;
    bool isDragging;
    bool isResizing;
    QPoint dragPosition;
    QPoint resizeStartPos;
    QRect originalGeometry;
    ResizeRegion currentRegion = None;
};
#endif // VIDEOREADER_H
