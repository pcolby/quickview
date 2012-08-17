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
    QString dirName, fileName;
    for (int index=1; (dirName.isEmpty()) && (index < app.arguments().size()); ++index) {
        QFileInfo fileInfo(app.arguments().at(index));
        if (fileInfo.exists()) {
            dirName = fileInfo.absolutePath();
            if (fileInfo.isFile())
                fileName = fileInfo.fileName();
        }
    }

    // Prompt for the directory to show images from.
    QSettings settings;
    if (dirName.isEmpty()) {
        dirName=settings.value(QLatin1String("directory")).toString();
        dirName=QFileDialog::getExistingDirectory(0,QObject::tr("Open Images Directory"),dirName);
    }
    if (dirName.isEmpty()) return 1;
    settings.setValue(QLatin1String("directory"),dirName);

    // Fetch the list of image files in the chosen directory.
    QStringList nameFilters;
    foreach (const QByteArray &format, formats)
        nameFilters << QString::fromLatin1("*.%1").arg(QString::fromLatin1(format).toLower());
    const QDir dir(dirName);
    const QStringList fileNames=dir.entryList(nameFilters,QDir::Files,QDir::Name);
    if (fileNames.isEmpty()) {
        QMessageBox::critical(0,QObject::tr("File not found"),
                              QObject::tr("No images where found in the directory %1")
                              .arg(QDir::toNativeSeparators(dirName)));
        return 2;
    }

    // If a filename was given, get the index of the selected file.
    int fileIndex=0;
    if (!fileName.isEmpty()) {
        fileIndex=-1;
        for (int index=0; (fileIndex < 0) && (index < fileNames.size()); ++index) {
            if (fileNames.at(index) == fileName)
                fileIndex = index;
        }
        if (fileIndex < 0) {
            QMessageBox::warning(0,QObject::tr("File not found"),QObject::tr("Could not find %1 in %2")
                                 .arg(QDir::toNativeSeparators(fileName), QDir::toNativeSeparators(dirName)));
            fileIndex=0;
        }
    }

    // Prompt for the image duration, unless a file was specified on the command line.
    int duration=settings.value(QLatin1String("duration"),4000).toInt();
    if (fileName.isEmpty()) {
        bool ok;
        duration=QInputDialog::getInt(0,QObject::tr("Duration"),QObject::tr("How long (in milliseonds) should each image display for?"),
                                      duration,100,2147483647,1,&ok);
        if (!ok) return 3;
        settings.setValue(QLatin1String("duration"),duration);
    }

    // Instantiate the main window.
    MainWindow *mainWindow=new MainWindow(dirName,fileNames,duration,fileIndex);
    mainWindow->show();
    return app.exec();
}
