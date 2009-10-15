#include "mainwindow.h"
#include <QApplication>
#include <QResizeEvent>
#include <QSettings>

MainWindow::MainWindow(const QString &dirName, const QStringList &fileNames, const int duration, QWidget *parent/*=0*/, Qt::WindowFlags flags/*=0*/)
        : QWidget(parent,flags), dirName(dirName), fileNames(fileNames) {
    Q_ASSERT(!fileNames.isEmpty());
    setWindowTitle(QString::fromAscii("%1 %2").arg(QApplication::applicationName()).arg(QApplication::applicationVersion()));

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

void MainWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
}

void MainWindow::timerEvent(QTimerEvent *event) {
    loadNextImage();
}

/* Private functions */

bool MainWindow::loadNextImage() {
    return false; // Not implemented yet!
}
