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

#ifndef __PATH_PAGE__
#define __PATH_PAGE__

#include <QFileDialog>
#include <QWizardPage>

class PathPage : public QWizardPage {
    Q_OBJECT

  public:
    PathPage(QWidget *parent=0);
    virtual bool isComplete() const;

  public slots:
    void save();

  protected:
    QFileDialog * fileDialog;

    virtual void showEvent(QShowEvent *event);

  protected slots:
    void emitCompleteChanged();
    void pathSelected(const QString &path);
};

#endif // __PATH_PAGE__
