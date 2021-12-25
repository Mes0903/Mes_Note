#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QErrorMessage>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT

  public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QTextEdit *te_test;

    QPushButton *bt_filename;
    QPushButton *bt_getcolor;
    QPushButton *bt_getfont;
    QPushButton *bt_getinput;
    QPushButton *bt_message;
    QPushButton *bt_error;
    QPushButton *bt_progress;


  private:
    Ui::MainWindow *ui;
};
#endif    // MAINWINDOW_H
