#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QWidget>
#include <QFileInfo>

class MainWindow : public QWidget { Q_OBJECT
  public:
    MainWindow(QWidget *parent=0, Qt::WindowFlags flags=0);

  public slots:
    bool pause();
    bool play();
    void setDuration(const int duration);
    int setPath(const QFileInfo &fileInfo);

  protected:
    int duration, timerId;
    QFileInfoList filesToShow;
    QFileInfoList::const_iterator currentFile;
    QPixmap pixmap, pixmapScaled;
    QPoint pixmapOffset;
    QRect pixmapRect;

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
    void scalePixmap(const bool force=false);
};

#endif // __MAIN_WINDOW_H__
