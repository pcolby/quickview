#ifndef __PATH_PAGE__
#define __PATH_PAGE__

#include <QFileDialog>
#include <QWizardPage>

class PathPage : public QWizardPage { Q_OBJECT
  public:
    PathPage(QWidget *parent=0);
    virtual bool isComplete() const;
    //virtual bool isComplete(const QString &pathName) const;

  protected:
    QFileDialog * fileDialog;

  protected slots:
    void emitCompleteChanged();
    void pathSelected(const QString &path);
};

#endif // __PATH_PAGE__
