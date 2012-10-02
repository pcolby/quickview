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
}
