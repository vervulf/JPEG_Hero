#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPixmap>
#include <QListView>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QString>
#include <QScrollArea>
#include <QRect>
#include <QSize>
#include <QDesktopWidget>
#include <QMessageBox>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPixmap *img;
    QString *img_path;
    QScrollArea *scrollArea;
    bool img_fits_wnd, img_autoupdate;
    int LISTVIEW_WIDTH;

    //QLabel *img_label;
    //QListView *clusters_lv;

    bool eventFilter(QObject *watched, QEvent *event);

private slots:
    void get_img();
    void fit_size();
    void autoupdate();
    void wnd_resize();
    void save_file();

signals:
    void wnd_resize_sig();

};

#endif // MAINWINDOW_H
