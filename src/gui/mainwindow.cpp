#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QImageReader>
#include <QPainter>
#include <QResizeEvent>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags): QWidget(parent,flags) {
    setAttribute(Qt::WA_NoSystemBackground,true);
    setAttribute(Qt::WA_OpaquePaintEvent,true);

    // Start the load timer.
    duration=4000;
    timerId=startTimer(duration);

    // Set the window title.
    updateWindowTitle();

    // Restore the window's size and position.
    QSettings settings;
    QVariant geometry=settings.value(QLatin1String("geometry"));
    if (geometry.isValid()) restoreGeometry(geometry.toByteArray());
    else setGeometry(40,40,1024,750); // 1024x750 at position (40,40).
}

/* Public slots */

void MainWindow::loadNextImage() {
    if (filesToShow.isEmpty())
        return;
    if ((++currentFile)==filesToShow.constEnd())
        currentFile==filesToShow.constBegin();
    loadImage(*currentFile);
}

void MainWindow::loadPreviousImage() {
    if (filesToShow.isEmpty())
        return;
    if (currentFile==filesToShow.constBegin())
        currentFile==filesToShow.constEnd();
    loadImage(*--currentFile);
}

void MainWindow::setDuration(const int duration) {
    this->duration=duration;
}

int MainWindow::setPath(const QFileInfo &fileInfo) {
    Q_ASSERT(fileInfo.exists());
    const QDir dir = (fileInfo.isDir()) ? QDir(fileInfo.absoluteFilePath()) : fileInfo.dir();

    // Get list of image file formats supported by the installed Qt plugins.
    const QList<QByteArray> formats=QImageReader::supportedImageFormats();

    // Fetch the list of image files in the chosen directory.
    QStringList nameFilters;
    foreach (const QByteArray &format, formats)
        nameFilters << QString::fromLatin1("*.%1").arg(QString::fromLatin1(format).toLower());
    filesToShow=dir.entryInfoList(nameFilters,QDir::Files,QDir::Name);
    currentFile=(filesToShow.isEmpty()) ? filesToShow.constEnd() : filesToShow.constBegin();

    // If fileInfo is an actual file (as opposed to a directory), skip straight to the specified file.
    if ((fileInfo.isFile()) && (!filesToShow.empty())) {
        currentFile=filesToShow.constEnd();
        for (QFileInfoList::ConstIterator iter=filesToShow.constBegin(); (currentFile==filesToShow.end()) && (iter!=filesToShow.end()); ++iter) {
            if (*iter==fileInfo)
                currentFile=iter;
        }
        if (currentFile==filesToShow.constEnd())
            currentFile=filesToShow.constBegin();
    }

    // Load the first image.
    if (!filesToShow.isEmpty())
        loadImage(*currentFile);
    return filesToShow.size();
}

/* Qt event overrides */

void MainWindow::closeEvent(QCloseEvent *event) {
    QSettings settings;
    settings.setValue(QLatin1String("geometry"),saveGeometry());
    QWidget::closeEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    // Remember: widgets should not override keyPressEvent without also overriding keyReleaseEvent accordingly.
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
        case Qt::Key_Left:
        case Qt::Key_Z:
            loadPreviousImage();
            break;
        case Qt::Key_P: // Pause/un-pause the slideshow (ie fall-through).
        case Qt::Key_S: // Start/stop the slideshow.
        case Qt::Key_Space: // Advance to the next image.
            if (timerId==0) // Start.
                timerId=startTimer(duration);
            else { // Stop.
                killTimer(timerId);
                timerId=0;
            }
            updateWindowTitle();
            break;
        case Qt::Key_Right:
        case Qt::Key_X:
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
        case Qt::Key_Left:   // fall-through.
        case Qt::Key_Z:
        case Qt::Key_P:      // fall-through.
        case Qt::Key_S:      // fall-through.
        case Qt::Key_Space:
        case Qt::Key_Right:
        case Qt::Key_X:
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
    // If we have a loaded file, show the filename first.
    QString title;
    if (!filesToShow.isEmpty())
        title += QString::fromLatin1("%1 - ").arg(currentFile->fileName());

    // Indicate the "paused" state (if paused).
    if (timerId == 0)
        title += tr("PAUSED - ");

    // Include the application name.
    title += QApplication::applicationName() + QLatin1Char(' ');

    // Include the version string.
    #ifdef DEBUG
    QRegExp versionMatch(QLatin1String("([^.]+\\.[^.]+\\.[^.]+\\.[^.]+)"));
    #else
    QRegExp versionMatch(QLatin1String("([^.]+\\.[^.]+\\.[^.]+)\\."));
    #endif // DEBUG
    title +=(QApplication::applicationVersion().contains(versionMatch)) ? versionMatch.cap(1) : QApplication::applicationVersion();

    // Set the window title.
    setWindowTitle(title);
}

/* Private functions */

void MainWindow::loadImage(const QFileInfo &fileInfo) {
    // Kill any current timers first.
    const bool wasRunning=(timerId!=0);
    if (wasRunning) {
        killTimer(timerId);
        timerId=0;
    }

    // Load the image.
    pixmap.load(fileInfo.absoluteFilePath());

    // Scale, and paint the new image.
    scalePixmap(true);
    repaint();

    // Re-start the load timer (if we stopped it above).
    if (wasRunning)
        timerId=startTimer(duration);

    // Update the window title.
    updateWindowTitle();
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
