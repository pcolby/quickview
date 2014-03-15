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

#include "pathpage.h"
#include "settings.h"
#include <QHBoxLayout>
#include <QImageReader>
#include <QSettings>
#include <QTimer>
#include <QVariant>

PathPage::PathPage(QWidget *parent): QWizardPage(parent), fileDialog(NULL) {
    setTitle(tr("Select Path To Show"));
    setSubTitle(tr("Select the path to show images from."));

    QHBoxLayout * const layout = new QHBoxLayout;

    QStringList filters;
    foreach (const QByteArray &format, QImageReader::supportedImageFormats())
        filters.append(QString::fromLatin1("*.%1 ").arg(QString::fromLatin1(format).toLower()));
    filters.sort();
    QString filter = QString::fromLatin1("Images (%1)").arg(filters.join(QLatin1String(" ")));

    fileDialog = new QFileDialog(0, QString(), QString(), filter);
    fileDialog->setFileMode(QFileDialog::Directory);
    fileDialog->setOption(QFileDialog::ReadOnly);
    layout->addWidget(fileDialog);

    QSettings settings;
    QVariant pathName = settings.value(Setting::PathName);
    if (pathName.isValid()) {
        fileDialog->selectFile(pathName.toString());
    }

    setLayout(layout);

    connect(fileDialog, SIGNAL(currentChanged(QString)), this, SLOT(pathSelected(QString)));
    connect(fileDialog, SIGNAL(directoryEntered(QString)), this, SLOT(pathSelected(QString)));
}

bool PathPage::isComplete() const {
    if (fileDialog == 0) return false;

    const QStringList selectedFiles = fileDialog->selectedFiles();
    if (selectedFiles.isEmpty()) return false;

    QFileInfo fileInfo(selectedFiles.first());
    if (!fileInfo.isReadable()) return false;
    if (fileInfo.isFile()) return true;

    QStringList nameFilters;
    foreach (const QByteArray &format, QImageReader::supportedImageFormats())
        nameFilters << QString::fromLatin1("*.%1").arg(QString::fromLatin1(format).toLower());

    const QDir dir(fileInfo.absoluteFilePath());
    return (!dir.entryList(nameFilters,QDir::Files,QDir::Unsorted).isEmpty());
}

void PathPage::save() {
    QSettings settings;
    settings.setValue(Setting::PathName, QDir::toNativeSeparators(fileDialog->selectedFiles().first()));
}

void PathPage::showEvent(QShowEvent *event) {
    foreach (QObject * const child, fileDialog->children()) {
        const QLatin1String childClassName(child->metaObject()->className());
        if ((childClassName == QLatin1String("QDialogButtonBox")) || (childClassName == QLatin1String("QSizeGrip")))
            qobject_cast<QWidget *>(child)->setVisible(false);
        else if ((childClassName == QLatin1String("QLabel")) && (child->objectName() == QLatin1String("fileTypeLabel")))
            qobject_cast<QWidget *>(child)->setVisible(false);
        else if ((childClassName == QLatin1String("QComboBox")) && (child->objectName() == QLatin1String("fileTypeCombo")))
            qobject_cast<QWidget *>(child)->setVisible(false);
    }
}

void PathPage::pathSelected(const QString &path) {
    Q_UNUSED(path)

    // Give the dialog time to update before emitting the completeChanged signal.
    QTimer::singleShot(0, this, SLOT(emitCompleteChanged()));
}

void PathPage::emitCompleteChanged() {
    emit completeChanged();
}
