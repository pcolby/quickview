#include "viewoptionspage.h"
#include "mainwindow.h"
#include "settings.h"
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QSpinBox>

ViewOptionsPage::ViewOptionsPage(QWidget *parent): QWizardPage(parent) {
    setTitle(tr("View Options"));
    setSubTitle(tr("Choose how the images should be shown."));

    QFormLayout *layout=new QFormLayout;
    QSettings settings;

    QHBoxLayout * const intervalLayout = new QHBoxLayout;
    QSpinBox * const duration = new QSpinBox;
    duration->setRange(100, 24 * 60 * 60 * 1000);
    duration->setValue(settings.value(Setting::SlideShowDuration, 4000).toInt());
    intervalLayout->addWidget(duration);
    intervalLayout->addWidget(new QLabel(tr("milliseconds")));
    intervalLayout->addStretch();
    layout->addRow(tr("Slide interval:"), intervalLayout);
    registerField(Setting::SlideShowDuration, duration);

    QHBoxLayout * const zoomModeLayout = new QHBoxLayout;
    QComboBox * const zoomMode = new QComboBox;
    zoomMode->addItem(tr("Explcit Scale"),    MainWindow::ExplicitScale);
    zoomMode->addItem(tr("Shrink to Window"), MainWindow::ShrinkToWindow);
    zoomMode->addItem(tr("Zoom to Window"),   MainWindow::ZoomToWindow);
    zoomMode->setCurrentIndex(zoomMode->findText(settings.value(Setting::ZoomMode).toString()));
    registerField(Setting::ZoomMode, zoomMode, "currentText");
    QDoubleSpinBox * const explicitScale = new QDoubleSpinBox;
    explicitScale->setDecimals(3);
    explicitScale->setRange(1E-3, 2048.0);
    explicitScale->setValue(settings.value(Setting::ExplicitScale, 1.0).toDouble());
    zoomModeLayout->addWidget(zoomMode);
    zoomModeLayout->addWidget(explicitScale);
    zoomModeLayout->addStretch();
    layout->addRow(tr("Zoom mode:"), zoomModeLayout);
    registerField(Setting::ExplicitScale, explicitScale);

    setLayout(layout);
}

void ViewOptionsPage::save() {
    QSettings settings;
    settings.setValue(Setting::SlideShowDuration, field(Setting::SlideShowDuration));
    settings.setValue(Setting::ZoomMode,          field(Setting::ZoomMode));
    settings.setValue(Setting::ExplicitScale,     field(Setting::ExplicitScale));
}
