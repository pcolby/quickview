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

#include "optionswizard.h"
#include "pathpage.h"
#include "viewoptionspage.h"
#include <QApplication>
#include <QMessageBox>
#include <QVariant>

const QString OptionsWizard::PathNameField(QString::fromLatin1("abc"));

OptionsWizard::OptionsWizard(QWidget *parent, Qt::WindowFlags flags): QWizard(parent,flags) {
    setWindowTitle(QString::fromLatin1("%1 %2").arg(QApplication::applicationName())
                   .arg(QStringList(QApplication::applicationVersion().split(QLatin1Char('.')).mid(0,3)).join(QLatin1String("."))));

    PathPage * const pathPage = new PathPage;
    pathPage->setFinalPage(true);
    addPage(pathPage);

    ViewOptionsPage * const viewOptionsPage = new ViewOptionsPage;
    addPage(viewOptionsPage);

    setButtonText(FinishButton, tr("Show"));

    //setOption(NoBackButtonOnLastPage);//, false);
    setOption(NoBackButtonOnStartPage, false);
    //setOption(DisabledBackButtonOnLastPage, false);//, true);
    setOption(NoDefaultButton);

    connect(this, SIGNAL(accepted()), pathPage, SLOT(save()));
    connect(this, SIGNAL(accepted()), viewOptionsPage, SLOT(save()));

    next();
}
