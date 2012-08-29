#include "gui/mainwindow.h"
#include "os/versioninfo.h"
#include <QApplication>
#include <QFileDialog>
#include <QImageReader>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>

#define APPLICATION_NAME    QLatin1String("SlideShow")   // Note, these values are used by the
#define ORGANISATION_NAME   QLatin1String("Paul Colby")  // QSettings default constructor, so
#define ORGANISATION_DOMAIN QLatin1String("colby.id.au") // should not change between versions.

#ifdef DEBUG
#include "os/filehandlerinfo.h"
#endif

int main(int argc, char *argv[]) {
    // Setup the primary Qt application object.
    QApplication app(argc, argv);
    app.setApplicationName(APPLICATION_NAME);
    app.setOrganizationName(ORGANISATION_NAME);
    app.setOrganizationDomain(ORGANISATION_DOMAIN);
    app.setApplicationVersion(VersionInfo::getAppVersionStr());

    // Get list of image file formats supported by the installed Qt plugins.
    const QList<QByteArray> formats=QImageReader::supportedImageFormats();

    // Check the command line options.
#ifdef Q_OS_WIN
    const FileHandlerInfo::UserScope userScope =
        (app.arguments().indexOf(QLatin1String("-all-users")) == -1) ? FileHandlerInfo::CurrentUser : FileHandlerInfo::AllUsers;
    if (app.arguments().indexOf(QLatin1String("-set-open-with-default")) != -1) {
        foreach (QByteArray format, formats) {
            FileHandlerInfo::setOpenWithDefault(QString::fromLatin1(format).toLower(), userScope);
        }
        return 0;
    } else if (app.arguments().indexOf(QLatin1String("-enable-open-with")) != -1) {
        foreach (QByteArray format, formats) {
            FileHandlerInfo::enableOpenWith(QString::fromLatin1(format).toLower(), userScope);
        }
        return 0;
    }
#endif

    // Find the first argument that identifies an existing file or directory.
    QFileInfo fileInfo;
    for (int index=1; (!fileInfo.exists()) && (index < app.arguments().size()); ++index) {
        fileInfo = QFileInfo(app.arguments().at(index));
    }

    QSettings settings;
    int duration=settings.value(QLatin1String("duration"),4000).toInt();

    // Prompt for the directory to show images from.
    if (!fileInfo.exists()) {
        QString dirName=settings.value(QLatin1String("directory")).toString();
        dirName=QFileDialog::getExistingDirectory(0,QObject::tr("Open Images Directory"),dirName);
        if (dirName.isEmpty()) return 1;
        settings.setValue(QLatin1String("directory"),dirName);
        fileInfo=QFileInfo(dirName);

        // Prompt for the image duration, unless a file was specified on the command line.
        bool ok;
        duration=QInputDialog::getInt(0,QObject::tr("Duration"),QObject::tr("How long (in milliseonds) should each image display for?"),
                                      duration,100,2147483647,1,&ok);
        if (!ok) return 3;
        settings.setValue(QLatin1String("duration"),duration);
    }

    // Instantiate the main window.
    MainWindow *mainWindow=new MainWindow;
    mainWindow->setPath(fileInfo);
    mainWindow->show();
    return app.exec();
}
