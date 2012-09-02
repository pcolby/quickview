#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QWidget>
#include <QFileInfo>

class MainWindow : public QWidget { Q_OBJECT
  public:
    typedef enum {
        ExplicitScale,
        ShrinkToWindow,
        ZoomToWindow
    } ZoomMode;

    MainWindow(QWidget *parent=0, Qt::WindowFlags flags=0);

  public slots:
    bool pause();
    bool play();
    bool playPause();
    void setDuration(const int duration);
    int setPath(const QFileInfo &fileInfo);
    void shrinkToWindow();
    void toggleFullscreen();
    void zoomIn(const float scale=0.10);
    void zoomOut(const float scale=0.10);
    void zoomTo(const float scale);
    void zoomToWindow();

  protected:
    int duration, timerId;
    QFileInfoList filesToShow;
    QFileInfoList::const_iterator currentFile;
    QPixmap pixmap, pixmapScaled;
    QPoint pixmapOffset;
    QRect pixmapRect;
    ZoomMode zoomMode;
    float scale;    ///< Scale to use when zoomMode is ExplicitScale.
    float scaleMin; ///< Minimum scale to go down to for the current pixmap.

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);   // Remember: widgets should not override keyPressEvent
    void keyReleaseEvent(QKeyEvent *event); // without also overriding keyReleaseEvent accordingly.
    void mouseDoubleClickEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void timerEvent(QTimerEvent *event);

  protected slots:
    void loadNextImage();
    void loadPreviousImage();
    void updateWindowTitle();

  private:
    void loadImage(const QFileInfo &fileInfo);
    void rescale();
};

#endif // __MAIN_WINDOW_H__
