#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "include_libs.h"
#include "clusterdlg.h"


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
    QString appPath,
            tmpPath,
            backupPath,
            tempFilePath;
    QString file_types;
    Ui::MainWindow *ui;
    QString *imgPath; //current img path
    QWidget *imgView; //ImgView
    bool img_autoupdate;
    int LISTVIEW_WIDTH;
    QLabel *status_lbl;
    QSet<unsigned int>  *itemSet,
                        *delClusters; // numbers of cluster to remove (disable)
    QList<QListWidgetItem*> *itemList; // checkboxes in ListView
    int fileClusters; //total clusters ammount
    QList<QByteArray> *clusters_list; //clusters list with CLUSTER_SIZE bytes element size
    QProcess *native_viewer,
             *getWnd;
    QWindow *nativeWindow;
    QString program,
            argument;

    int countClusters();    
    QList<unsigned int> parse_clusters_str(QString str);


private slots:
    void open_file();
    void autoupdate();
    void save_file();
    void save_file_as();
    void update_view(int);
    void update_file();
    void resotre_file();
    void cluster_clicked(QListWidgetItem *);
    void add_cluster();
    void remove_cluster();
    void add_to_list(QString&);
    void remove_from_list(QString&);


signals:
    void sig_update_file();
    void sig_update_view(int);


};

#endif // MAINWINDOW_H
