/*
 *  Copyright 2009-2014 Paul Colby
 *
 *  This file is part of QuickView.
 *
 *  QuickView is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  QuickView is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with QuickView.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    QPoint pixmapOffset, moveOffset;
    QRect pixmapRect;
    ZoomMode zoomMode;
    float scale;    ///< Scale to use when zoomMode is ExplicitScale.
    float scaleMin; ///< Minimum value for \a scale (calculated by loadImage).

    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);   // Remember: widgets should not override keyPressEvent
    void keyReleaseEvent(QKeyEvent *event); // without also overriding keyReleaseEvent accordingly.
    void mouseDoubleClickEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void timerEvent(QTimerEvent *event);

  protected slots:
    void arrowKeyPressEvent(QKeyEvent *event);
    void loadNextImage();
    void loadPreviousImage();
    void moveImageDown(const int by=10);
    void moveImageLeft(const int by=10);
    void moveImageRight(const int by=10);
    void moveImageUp(const int by=10);
    void updateWindowTitle();

  private:
    void loadImage(const QFileInfo &fileInfo);
    void rescale();
};

#endif // __MAIN_WINDOW_H__
