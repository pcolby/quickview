#include "mainwindow.h"
#include <QApplication>
#include <QPainter>
#include <QResizeEvent>
#include <QSettings>

MainWindow::MainWindow(const QString &dirName, const QStringList &fileNames, const int duration, QWidget *parent/*=0*/, Qt::WindowFlags flags/*=0*/)
        : QWidget(parent,flags), dirName(dirName), fileNames(fileNames), fileNamesIndex(-1) {
    Q_ASSERT(!fileNames.isEmpty());
    setWindowTitle(QString::fromAscii("%1 %2").arg(QApplication::applicationName()).arg(QApplication::applicationVersion()));

    // Load the first image.
    loadNextImage();

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
        case Qt::Key_F:
            if (isFullScreen())
                showNormal();
            else
                showFullScreen();
            event->setAccepted(true);
            break;
        default:
            QWidget::keyPressEvent(event);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_F:
            event->setAccepted(true);
            break;
        default:
            QWidget::keyReleaseEvent(event);
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    // Calculate the source and destination rectangles.
    QRect targetRect=event->rect().intersected(pixmapRect);
    QRect sourceRect=targetRect.translated(-pixmapOffset);
    Q_ASSERT(sourceRect.size()==targetRect.size());

    // Paint the pixmap.
    QPainter painter(this);
    painter.save();
    painter.drawPixmap(targetRect,pixmapScaled,sourceRect);
    painter.restore();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    scalePixmap();
    QWidget::resizeEvent(event);
}

void MainWindow::timerEvent(QTimerEvent *event) {
    loadNextImage();
}

/* Private functions */

void MainWindow::loadNextImage() {
    fileNamesIndex++; // Move to the next image.
    if (fileNamesIndex>=fileNames.count())
        fileNamesIndex=0; // Repeat all ;)
    pixmap.load(QString::fromAscii("%1/%2").arg(dirName).arg(fileNames.at(fileNamesIndex)));
    scalePixmap(true);
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
