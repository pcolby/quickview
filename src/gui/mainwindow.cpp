#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QIcon>
#include <QImageReader>
#include <QPainter>
#include <QResizeEvent>
#include <QSettings>

#define SCALE_MAX 2048.0 // Enough to make one single pixel fill a 4K screen!!

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags): QWidget(parent,flags), timerId(0), zoomMode(ZoomToWindow) {
    setAttribute(Qt::WA_NoSystemBackground,true);
    setAttribute(Qt::WA_OpaquePaintEvent,true);

    // Set the window title.
    updateWindowTitle();

    // Restore the window's size and position.
    QSettings settings;
    QVariant geometry=settings.value(QLatin1String("geometry"));
    if (geometry.isValid()) restoreGeometry(geometry.toByteArray());
    else setGeometry(40,40,1024,750); // 1024x750 at position (40,40).
}

/* Public slots */

bool MainWindow::pause() {
    if (timerId==0)
        return false; // Was not previously running.
    killTimer(timerId);
    timerId=0;
    updateWindowTitle();
    return true; // Was previously running.
}

bool MainWindow::play() {
    if (timerId!=0)
        return true; // Was previously running.
    timerId=startTimer(duration);
    updateWindowTitle();
    return false; // Was not previously running.
}

bool MainWindow::playPause() {
    return (timerId==0) ? play() : pause();
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
    filesToShow=dir.entryInfoList(nameFilters,QDir::Files,QDir::Name|QDir::IgnoreCase|QDir::LocaleAware);
    currentFile=(filesToShow.isEmpty()) ? filesToShow.constEnd() : filesToShow.constBegin();

    // If fileInfo is an actual file (as opposed to a directory), skip straight to the specified file.
    if ((fileInfo.isFile()) && (!filesToShow.empty())) {
        currentFile=filesToShow.constEnd();
        for (QFileInfoList::ConstIterator iter=filesToShow.constBegin(); (currentFile==filesToShow.constEnd()) && (iter!=filesToShow.constEnd()); ++iter) {
            if (*iter==fileInfo)
                currentFile=iter;
        }
        if (currentFile==filesToShow.constEnd())
            currentFile=filesToShow.constBegin();
    } else play();

    // Load the first image.
    if (!filesToShow.isEmpty())
        loadImage(*currentFile);
    return filesToShow.size();
}

void MainWindow::shrinkToWindow() {
    zoomMode=ShrinkToWindow;
    rescale();
}

void MainWindow::toggleFullscreen() {
    if (isFullScreen()) showNormal();
    else showFullScreen();
}

void MainWindow::zoomIn(const float scale) {
    if (zoomMode!=ExplicitScale)
        this->scale=pixmapScaled.size().height()/static_cast<float>(pixmap.size().height());
    zoomTo(this->scale*(1.0+scale));
}

void MainWindow::zoomOut(const float scale) {
    if (zoomMode!=ExplicitScale)
        this->scale=pixmapScaled.size().height()/static_cast<float>(pixmap.size().height());
    zoomTo(this->scale*(1.0-scale));
}

void MainWindow::zoomTo(const float scale) {
    zoomMode=ExplicitScale;
    this->scale=qBound<float>(scaleMin,scale,SCALE_MAX);
    rescale();
}

void MainWindow::zoomToWindow() {
    zoomMode=ZoomToWindow;
    rescale();
    update();
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
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:        zoomTo(event->key()-Qt::Key_0); break;
        case Qt::Key_Asterisk: zoomToWindow();                 break;
        case Qt::Key_Backslash:zoomToWindow();                 break;
        case Qt::Key_Escape:   close();                        break;
        case Qt::Key_Equal:    zoomOut();                      break;
        case Qt::Key_F:        toggleFullscreen();             break;
        case Qt::Key_Left:     loadPreviousImage();            break;
        case Qt::Key_Minus:    zoomOut();                      break;
        case Qt::Key_P:        playPause();                    break;
        case Qt::Key_Plus:     zoomIn();                       break;
        case Qt::Key_Right:    loadNextImage();                break;
        case Qt::Key_S:        playPause();                    break;
        case Qt::Key_Slash:    shrinkToWindow();               break;
        case Qt::Key_Space:    playPause();                    break;
        case Qt::Key_X:        loadNextImage();                break;
        case Qt::Key_Z:        loadPreviousImage();            break;
        default:
            QWidget::keyPressEvent(event);
            return;
    }
    event->setAccepted(true);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
        case Qt::Key_Asterisk:
    case Qt::Key_Backslash:
        case Qt::Key_Escape:
        case Qt::Key_Equal:
        case Qt::Key_F:
        case Qt::Key_Left:
        case Qt::Key_P:
        case Qt::Key_Plus:
        case Qt::Key_Minus:
        case Qt::Key_Right:
        case Qt::Key_S:
        case Qt::Key_Slash:
        case Qt::Key_Space:
        case Qt::Key_X:
        case Qt::Key_Z:
            event->setAccepted(true);
            break;
        default:
            QWidget::keyReleaseEvent(event);
    }
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {
    toggleFullscreen();
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
    if (zoomMode==ZoomToWindow)
        rescale();
    QWidget::resizeEvent(event);
}

void MainWindow::timerEvent(QTimerEvent *event) {
    if (event->timerId()==timerId) {
        event->setAccepted(true);
        loadNextImage();
    } else QObject::timerEvent(event);
}

/* Protected slots */

void MainWindow::loadNextImage() {
    if (filesToShow.isEmpty())
        return;
    if ((++currentFile)==filesToShow.constEnd())
        currentFile=filesToShow.constBegin();
    loadImage(*currentFile);
}

void MainWindow::loadPreviousImage() {
    if (filesToShow.isEmpty())
        return;
    if (currentFile==filesToShow.constBegin())
        currentFile=filesToShow.constEnd();
    loadImage(*--currentFile);
}

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
    const bool wasRunning=pause();

    // Load the image.
    if (pixmap.load(fileInfo.absoluteFilePath())) {
        scaleMin=16.0/qMin(pixmap.height(),pixmap.width());
        if (scale<scaleMin)
            scale=scaleMin;
    }

    // Scale, and paint (if necessary) the new image.
    rescale();

    // Re-start the load timer (if we stopped it above).
    if (wasRunning)
        play();

    // Update the window title.
    updateWindowTitle();
}

void MainWindow::rescale() {
    // Special case to handle a null pixmap (in case we have an image load fault).
    if (pixmap.isNull()) {
        if (!pixmapScaled.isNull())
            pixmapScaled=QPixmap();
        return; // Nothing to scale.
    }

    switch (zoomMode) {
        case ExplicitScale:
            Q_ASSERT(scale!=0);
            pixmapScaled=pixmap.scaled(pixmap.size()*scale,Qt::KeepAspectRatio);
            if (!isFullScreen())
                resize(pixmap.size()*scale);
            break;
        case ShrinkToWindow:
            pixmapScaled=((pixmap.size().height()<=size().height()) && (pixmap.size().width()<=size().width()))
                    ? pixmap : pixmap.scaled(size(),Qt::KeepAspectRatio);
            break;
        case ZoomToWindow:
            pixmapScaled=pixmap.scaled(size(),Qt::KeepAspectRatio);
            break;
    }

    // Update the pixmapOffset and pixmapRect propeties.
    const int heightDiff=height()-pixmapScaled.height();
    const int widthDiff=width()-pixmapScaled.width();
    //Q_ASSERT((heightDiff>=0)&&(widthDiff>=0));
    pixmapOffset=QPoint(widthDiff/2,heightDiff/2);           // These two member variables are calculated here
    pixmapRect=pixmapScaled.rect().translated(pixmapOffset); // so that paintEvent can be as fast as possible.
    update(); // Schedule repaint, unless hidden, etc.
}
