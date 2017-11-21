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
    QString *img_path, last_file_path = "C://";
    QScrollArea *scrollArea;

    //QLabel *img_label;
    //QListView *clusters_lv;

private slots:
    void get_img();

};

#endif // MAINWINDOW_H
