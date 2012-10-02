#ifndef __VIEW_OPTIONS_PAGE__
#define __VIEW_OPTIONS_PAGE__

#include <QWizardPage>

class ViewOptionsPage : public QWizardPage {
    Q_OBJECT

  public:
    ViewOptionsPage(QWidget *parent=0);

  public slots:
    void save();
};

#endif // __VIEW_OPTIONS_PAGE__
