#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "include_libs.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void get_app_path(QString path)
    {
        appPath = path;
    }

private:
    QString appPath, tmpPath, backupPath, tempFilePath;
    QString file_types;
    Ui::MainWindow *ui;
    QPixmap *img;
    QString *imgPath;
    QLabel *imgLabel;
    bool img_fits_wnd, img_autoupdate;
    int LISTVIEW_WIDTH;
    QList<QListWidgetItem*> *itemList;

private slots:
    void open_file();
    void fit_size();
    void autoupdate();
    void wnd_resize();
    void save_file();
    void save_file_as();
    void update_view();
    void update_file();
    void resotre_file();
    void cluster_clicked(QListWidgetItem *);

signals:
    void sig_update_file();
    void sig_update_view();


};

#endif // MAINWINDOW_H
