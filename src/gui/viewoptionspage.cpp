#include "viewoptionspage.h"
#include "settings.h"
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QSpinBox>

ViewOptionsPage::ViewOptionsPage(QWidget *parent): QWizardPage(parent) {
    setTitle(tr("View Options"));
    setSubTitle(tr("Choose how the images should be shown."));

    QFormLayout *layout=new QFormLayout;

    QHBoxLayout * const intervalLayout = new QHBoxLayout;
    intervalLayout->addWidget(new QSpinBox());
    intervalLayout->addWidget(new QLabel(tr("seconds")));
    layout->addRow(tr("Slide interval:"), intervalLayout);

    QHBoxLayout * const zoomModeLayout = new QHBoxLayout;
    QComboBox * const zoomMode = new QComboBox;
    zoomMode->addItem(tr("Explcit Scale"));
    zoomMode->addItem(tr("Shrink to Window"));
    zoomMode->addItem(tr("Zoom to Window"));
    QSpinBox * const explicitScale = new QSpinBox;
    zoomModeLayout->addWidget(zoomMode);
    zoomModeLayout->addWidget(explicitScale);
    layout->addRow(tr("Zoom mode:"), zoomModeLayout);

    setLayout(layout);
}

void ViewOptionsPage::save() {

}
