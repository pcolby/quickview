#include "gui/mainwindow.h"
#include "os/versioninfo.h"
#include <QApplication>
#include <QFileDialog>
#include <QImageReader>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>

#define APPLICATION_NAME    QString::fromAscii("SlideShow")   // Note, these values are used by the
#define ORGANISATION_NAME   QString::fromAscii("Paul Colby")  // QSettings default constructor, so
#define ORGANISATION_DOMAIN QString::fromAscii("colby.id.au") // should not change between versions.

int main(int argc, char *argv[]) {
    // Setup the primary Qt application object.
    QApplication app(argc, argv);
    app.setApplicationName(APPLICATION_NAME);
    app.setOrganizationName(ORGANISATION_NAME);
    app.setOrganizationDomain(ORGANISATION_DOMAIN);
    app.setApplicationVersion(VersionInfo::getAppVersionStr());

    // Prompt for the directory to show images from.
    QSettings settings;
    QString dirName=settings.value(QString::fromAscii("directory")).toString();
    dirName=QFileDialog::getExistingDirectory(0,QObject::tr("Open Images Directory"),dirName);
    if (dirName.isEmpty()) return 1;
    settings.setValue(QString::fromAscii("directory"),dirName);

    // Fetch the list of image files in the chosen directory.
    QStringList nameFilters;
    const QList<QByteArray> formats=QImageReader::supportedImageFormats();
    foreach (QByteArray format, formats)
        nameFilters << QString::fromAscii("*.%1").arg(QString::fromAscii(format).toLower());
    const QDir dir(dirName);
    const QStringList fileNames=dir.entryList(nameFilters,QDir::Files,QDir::Name);
    if (fileNames.isEmpty()) {
        QMessageBox::critical(0,QObject::tr("File not found"),QObject::tr("No images where found in the directory %1").arg(dirName));
        return 2;
    }

    // Prompt for the image duration.
    int duration=settings.value(QString::fromAscii("duration"),3000).toInt();
    bool ok;
    duration=QInputDialog::getInt(0,QObject::tr("Duration"),QObject::tr("How long (in milliseonds) should each image display for?"),
                                  duration,100,2147483647,1,&ok);
    if (!ok) return 3;
    settings.setValue(QString::fromAscii("duration"),duration);

    // Instantiate the main window.
    MainWindow *mainWindow=new MainWindow(dirName,fileNames,duration);
    mainWindow->show();
    return app.exec();
}
