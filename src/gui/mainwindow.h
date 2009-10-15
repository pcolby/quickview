#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QWidget>

class MainWindow : public QWidget { Q_OBJECT
  private:
    QString dirName;
    QStringList fileNames;
    QPixmap pixmap;

  public:
    MainWindow(const QString &dirName, const QStringList &fileNames, const int duration, QWidget *parent=0, Qt::WindowFlags flags=0);

  protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);   // Remember: widgets should not override keyPressEvent
    void keyReleaseEvent(QKeyEvent *event); // without also overriding keyReleaseEvent accordingly.
    void resizeEvent(QResizeEvent *event);  // TODO: Remove this override?
    void timerEvent(QTimerEvent *event);

  private:
    bool loadNextImage();
};

#endif // __MAIN_WINDOW_H__
