#include "mainwindow.h"
#include <QApplication>
#include <QIcon>
#include <QPainter>
#include <QResizeEvent>
#include <QSettings>

MainWindow::MainWindow(const QString &dirName, const QStringList &fileNames, const int duration, QWidget *parent/*=0*/, Qt::WindowFlags flags/*=0*/)
        : QWidget(parent,flags), dirName(dirName), fileNames(fileNames), duration(duration), fileNamesIndex(-1), timerId(0) {
    Q_ASSERT(!fileNames.isEmpty());
    setAttribute(Qt::WA_NoSystemBackground,true);
    setAttribute(Qt::WA_OpaquePaintEvent,true);
    updateWindowTitle();

    // Load the first image.
    loadNextImage();

    // Start the load timer.
    timerId=startTimer(duration);

    // Restore the window's size and position.
    QSettings settings;
    QVariant geometry=settings.value(QString::fromAscii("geometry"));
    if (geometry.isValid()) restoreGeometry(geometry.toByteArray());
    else setGeometry(40,40,1024,750); // 1024x750 at position (40,40).
}

/* Qt event overrides */

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings;
    settings.setValue(QString::fromAscii("geometry"),saveGeometry());
    QWidget::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Escape: // Exit application.
            close();
            break;
        case Qt::Key_F: // Toggle fullscreen mode.
            if (isFullScreen())
                showNormal();
            else
                showFullScreen();
            event->setAccepted(true);
            break;
        case Qt::Key_P: // Pause/un-pause the slideshow (ie fall-through).
        case Qt::Key_S: // Start/stop the slideshow.
            if (timerId==0) // Start.
                timerId=startTimer(duration);
            else { // Stop.
                killTimer(timerId);
                timerId=0;
            }
            break;
        case Qt::Key_Space: // Advance to the next image.
            loadNextImage();
            break;
        default:
            QWidget::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Escape: // fall-through.
        case Qt::Key_F:      // fall-through.
        case Qt::Key_P:      // fall-through.
        case Qt::Key_S:      // fall-through.
        case Qt::Key_Space:
            event->setAccepted(true);
            break;
        default:
            QWidget::keyReleaseEvent(event);
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    if (isFullScreen())
        showNormal();
    else
        showFullScreen();
    event->setAccepted(true);
}

void MainWindow::paintEvent(QPaintEvent *event) {
    // Calculate the source and destination rectangles.
    QRect targetRect=event->rect().intersected(pixmapRect);
    QRect sourceRect=targetRect.translated(-pixmapOffset);
    Q_ASSERT(sourceRect.size()==targetRect.size());

    // Paint the pixmap.
    QPainter painter(this);
    painter.save();
    painter.fillRect(event->rect(),Qt::black);
    painter.drawPixmap(targetRect,pixmapScaled,sourceRect);
    painter.restore();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    scalePixmap();
    QWidget::resizeEvent(event);
}

void MainWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId()==timerId) {
        event->setAccepted(true);
        loadNextImage();
    } else QObject::timerEvent(event);
}

/* Protected slots */

void MainWindow::updateWindowTitle() {
    // Get the version string.
    #ifdef DEBUG
    QRegExp versionMatch(QLatin1String("([^.]+\\.[^.]+\\.[^.]+\\.[^.]+)"));
    #else
    QRegExp versionMatch(QLatin1String("([^.]+\\.[^.]+\\.[^.]+)\\."));
    #endif // DEBUG
    const QString versionStr=(QApplication::applicationVersion().contains(versionMatch)) ? versionMatch.cap(1) : QString::null;

    // Update the window title.
    if ((0 <= fileNamesIndex) && (fileNamesIndex < fileNames.size()))
        setWindowTitle(tr("%1 - %2 %3").arg(fileNames.at(fileNamesIndex)).arg(QApplication::applicationName()).arg(versionStr));
    else
        setWindowTitle(tr("%1 %2").arg(QApplication::applicationName()).arg(versionStr));
}

/* Private functions */

void MainWindow::loadNextImage() {
    // Kill any current timers first.
    const bool wasRunning=(timerId!=0);
    if (wasRunning) {
        killTimer(timerId);
        timerId=0;
    }

    // Load the next image.
    fileNamesIndex++; // Move to the next image.
    if (fileNamesIndex>=fileNames.count())
        fileNamesIndex=0; // Repeat all ;)
    pixmap.load(QString::fromAscii("%1/%2").arg(dirName).arg(fileNames.at(fileNamesIndex)));
    setWindowIcon(pixmap);
    updateWindowTitle();

    // Scale, and paint the new image.
    scalePixmap(true);
    repaint();

    // Re-start the load timer (if we stopped it above).
    if (wasRunning)
        timerId=startTimer(duration);
}

void MainWindow::scalePixmap(const bool force/*=false*/) {
    // Special case to handle a null pixmap (in case we have an image load fault).
    if (pixmap.isNull()) {
        if (!pixmapScaled.isNull())
            pixmapScaled=QPixmap();
        return; // Nothing to scale.
    }

    // Check if the image even needs scaling.
    if ((force!=true)&&(pixmapScaled.size()==size()))
        return; // Already scaled to the current size.

    // Scale the image (if necessary).
    if (pixmap.size()==size())
        pixmapScaled=pixmap; // No need to scale it.
    else
        pixmapScaled=pixmap.scaled(size(),Qt::KeepAspectRatio);

    // Update the pixmapOffset and pixmapRect propeties.
    const int heightDiff=height()-pixmapScaled.height();
    const int widthDiff=width()-pixmapScaled.width();
    Q_ASSERT((heightDiff>=0)&&(widthDiff>=0));
    pixmapOffset=QPoint(widthDiff/2,heightDiff/2);           // These two member variables are calculated here
    pixmapRect=pixmapScaled.rect().translated(pixmapOffset); // so that paintEvent can be as fast as possible.
}
