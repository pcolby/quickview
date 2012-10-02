#ifndef __OPTIONS_WIZARD__
#define __OPTIONS_WIZARD__

#include <QWizard>
#include <QLabel>
#include <QPushButton>

class OptionsWizard : public QWizard {
  Q_OBJECT

  public:
    static const QString PathNameField;

    OptionsWizard(QWidget *parent=0, Qt::WindowFlags flags=0);
};

#endif // __OPTIONS_WIZARD__
