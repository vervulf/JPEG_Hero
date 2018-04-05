
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    img(new QPixmap()),
    imgPath(new QString("/")),
    imgLabel(new QLabel()),
    img_fits_wnd(true),
    img_autoupdate(false),
    LISTVIEW_WIDTH(175),
    file_types(tr("JPEG File (*.jpg *.jpeg)")),
    itemSet(new QSet<unsigned int>),
    delClusters(new QSet<unsigned int>),
    itemList(new QList<QListWidgetItem*>),
    fileClusters(-1),
    status_lbl(new QLabel())
{
    ui->setupUi(this);
    setWindowTitle("JPEG Hero");
    ui->statusBar->addWidget(status_lbl);
    status_lbl->setText("No File Opened");
    ui->actionAutoupdate->setChecked(img_autoupdate);
    ui->actionImage_fits_window->setChecked(img_fits_wnd);
    ui->scrollArea->setWidget(imgLabel);

    QSize *wndSize = new QSize();
    *wndSize = this->size();

    int imgW = wndSize->width()-LISTVIEW_WIDTH,
        imgH = wndSize->height()-77, // 77 = bottom status bar height
        clustW = LISTVIEW_WIDTH,
        clustH = wndSize->height()-77;

    ui->clusters_listview->setGeometry(0,0,clustW,clustH);
    imgLabel->setGeometry(LISTVIEW_WIDTH,0,imgW,imgH);

    QObject::connect(ui->actionAutoupdate,SIGNAL(triggered(bool)),this,SLOT(autoupdate()));
    QObject::connect(ui->actionImage_fits_window,SIGNAL(triggered(bool)),this,SLOT(fit_size()));
    QObject::connect(ui->action_Open,SIGNAL(triggered(bool)),this,SLOT(open_file()));
    QObject::connect(ui->action_Save,SIGNAL(triggered(bool)),this,SLOT(save_file()));
    QObject::connect(ui->actionSave_file_as,SIGNAL(triggered(bool)),this,SLOT(save_file_as()));
    QObject::connect(ui->actionRestore_default_file,SIGNAL(triggered(bool)),this,SLOT(resotre_file()));
    QObject::connect(ui->actionUpdate_image,SIGNAL(triggered(bool)),this,SLOT(update_file()));
    QObject::connect(ui->clusters_listview,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(cluster_clicked(QListWidgetItem*)));
    QObject::connect(this,SIGNAL(sig_update_file()),this,SLOT(update_file()));
    QObject::connect(this,SIGNAL(sig_update_view()),this,SLOT(update_view()));
    QObject::connect(ui->action_Add_Cluster, SIGNAL(triggered(bool)), this, SLOT(add_cluster()));
    QObject::connect(ui->actionRe_move_Cluster,SIGNAL(triggered(bool)),this,SLOT(remove_cluster()));

}

MainWindow::~MainWindow()
{
    if(QFile(tempFilePath).exists())
        QFile(tempFilePath).remove();
    if(QFile(backupPath).exists())
        QFile(backupPath).remove();

    delete ui,img,imgPath,imgLabel;
}

int MainWindow::countClusters()
{
    int CLUSTER_SIZE = 4096;
    QFile file(tempFilePath);
    int fSize = file.size();
    fileClusters = file.size()/CLUSTER_SIZE;
    if (file.size()%CLUSTER_SIZE!=0)
        fileClusters++;
    status_lbl->setText(QString::number(fileClusters) + " clusters in file");
    if (!file.open(QIODevice::ReadOnly))
    {
        ui->statusBar->showMessage("Failed to load file.");
        return -1;
    }

    clusters_list = new QList<QByteArray>;

        while (!file.atEnd())
        {
            QByteArray cluster = file.read(CLUSTER_SIZE);
            clusters_list->push_back(cluster);
        }

    file.close();

    return fileClusters;
}

QList<unsigned int> MainWindow::parse_clusters_str(QString str)
{
    QStringList strList = str.split(',');
    QList<unsigned int> clusters;
    QRegExp re("\\d*");
    foreach (QString substr, strList) {
       bool valid_num = true;
       if(substr.contains(':'))
       {
           QStringList clusters_range = substr.split(':');
           foreach (QString num, clusters_range) {
              if(!re.exactMatch(num) || num == "")
                  valid_num = false;
           }
           if(!valid_num)
               continue;

           int first , last;
           if(clusters_range[0].toInt() < clusters_range[1].toInt())
           {
               first = clusters_range[0].toInt();
               last = clusters_range[1].toInt();
           }
           else
           {
               first = clusters_range[1].toInt();
               last = clusters_range[0].toInt();
           }
           for(int i = first; i<= last; ++i)
               clusters.append(i);
       }
       else
           if(!re.exactMatch(substr) || substr == "")
               valid_num = false;
           if(!valid_num)
               continue;
           clusters.append(substr.toInt());
    }

    return clusters;
}


void MainWindow::open_file()
{
    *imgPath = QFileDialog::getOpenFileName(this,tr("Open File"),
                                             *imgPath, file_types);
    if (*imgPath == NULL)
    {
        *imgPath = "/";
        return;
    }

    delete img;
    qDeleteAll(itemList->begin(),itemList->end());
    delClusters->clear();
    itemSet->clear();
    itemList->clear();
    ui->clusters_listview->clear();
//    //QImageReader img_rdr();
//    *img = QPixmap::fromImage(QImage(*imgPath));
    img = new QPixmap(*imgPath);
    if (img_fits_wnd)
    {
        int w = ui->scrollArea->width(),
            h = ui->scrollArea->height();
        imgLabel->setPixmap(img->scaled(w,h,Qt::KeepAspectRatio));
    }
    else
    {
        int w = img->width(),
            h = img->height();
        imgLabel->resize(w,h);
        imgLabel->setPixmap(*img);
    }

    tmpPath = appPath + "/tmp";

    if (!QDir(tmpPath).exists())
        QDir().mkdir(tmpPath);


    tempFilePath = tmpPath + "/temp.jpg";
    backupPath = tmpPath + "/backup.jpg";

    if(QFile(tempFilePath).exists())
        QFile(tempFilePath).remove();
    if(QFile(backupPath).exists())
        QFile(backupPath).remove();

    bool result;
    result = QFile::copy(*imgPath, tempFilePath);
    result = QFile::copy(*imgPath, backupPath);
    this->countClusters();
}

void MainWindow::fit_size()
{
    img_fits_wnd = ui->actionImage_fits_window->isChecked();

    if (img_fits_wnd)
    {
        int w = ui->scrollArea->width(),
            h = ui->scrollArea->height();
        imgLabel->setPixmap(img->scaled(w,h,Qt::KeepAspectRatio));
    }
    else
    {
        int w = img->width(),
            h = img->height();
        imgLabel->resize(w,h);
        imgLabel->setPixmap(*img);
    }
}

void MainWindow::autoupdate()
{
    img_autoupdate = ui->actionAutoupdate->isChecked();
}

void MainWindow::wnd_resize()
{
    QSize *wndSize = new QSize();
    *wndSize = this->size();

    int imgW = wndSize->width()-LISTVIEW_WIDTH,
        imgH = wndSize->height()-77, // 77 = bottom status bar height
        clustW = LISTVIEW_WIDTH,
        clustH = wndSize->height()-77;

    imgLabel->setGeometry(LISTVIEW_WIDTH,0,imgW,imgH);
}

void MainWindow::save_file()
{
    QFile(*imgPath).remove();
    QFile::copy(tempFilePath,*imgPath);
}

void MainWindow::save_file_as()
{
    QString savePath = QFileDialog::getSaveFileName(this,tr("Save File As"),
                                                    *imgPath, file_types);

    if (savePath == NULL)
        return;

    if (QFile(savePath).exists())
        QFile(savePath).remove();
    QFile::copy(tempFilePath,savePath);

}

void MainWindow::update_view()
{

    delete img;
    img = new QPixmap(tempFilePath);

    if (img_fits_wnd)
    {
        int w = ui->scrollArea->width(),
            h = ui->scrollArea->height();
        imgLabel->setPixmap(img->scaled(w,h,Qt::KeepAspectRatio));
    }
    else
    {
        int w = img->width(),
            h = img->height();
        imgLabel->resize(w,h);
        imgLabel->setPixmap(*img);
    }
}

void MainWindow::update_file()
{
    if(QFile(tempFilePath).exists())
        QFile(tempFilePath).remove();

    QFile file(tempFilePath);

    if (!file.open(QIODevice::WriteOnly))
        return;

    for(int i=0; i<clusters_list->size(); ++i)
    {   if(!delClusters->contains(i))
            file.write(clusters_list->at(i));
    }

    file.close();

    emit sig_update_view();
}

void MainWindow::resotre_file()
{
    bool result;
    if(QFile(tempFilePath).exists())
        QFile(tempFilePath).remove();

    result = QFile::copy(backupPath, tempFilePath);
    qDeleteAll(itemList->begin(),itemList->end());
    delClusters->clear();
    itemSet->clear();
    itemList->clear();
    ui->clusters_listview->clear();
    emit this->update_view();

}

void MainWindow::cluster_clicked(QListWidgetItem *item)
{
    Qt::CheckState state = item->checkState();
    switch(state)
    {
    case Qt::Checked: item->setCheckState(Qt::Unchecked);
        delClusters->insert(abs(item->text().toInt()));
        break;
    case Qt::Unchecked: item->setCheckState(Qt::Checked);
        delClusters->remove(abs(item->text().toInt()));
        break;
    }

    if (img_autoupdate)
    {
        emit sig_update_file();
    }

}

void MainWindow::add_cluster()
{
 ClusterDlg *dlg = new ClusterDlg();
 dlg->show();

 QObject::connect(dlg,SIGNAL(send_string(QString&)),this,SLOT(add_to_list(QString&)));
}

void MainWindow::remove_cluster()
{
    ClusterDlg *dlg = new ClusterDlg();
    dlg->show();

    QObject::connect(dlg,SIGNAL(send_string(QString&)),this,SLOT(remove_from_list(QString&)));
}

void MainWindow::remove_from_list(QString &str)
{
    if (str.isEmpty() || abs(str.toInt()) > fileClusters)
        return;

    qDeleteAll(itemList->begin(),itemList->end());
    itemList->clear();
    ui->clusters_listview->clear();

    QList<unsigned int> clusters;
    if(str == "*")
    {
        for(int i=0; i <= fileClusters; ++i)
            clusters.append(i);
    }
    else
    {
        clusters= parse_clusters_str(str);
    }

    foreach (int num, clusters) {
        itemSet->remove(abs(num));
    }

    QList<int> numbers;
    for(auto itr=itemSet->begin(); itr!=itemSet->end(); ++itr)
    {
        numbers.push_back(*itr);
    }
    qSort(numbers.begin(),numbers.end());

    for(auto itr=numbers.begin(); itr!=numbers.end(); ++itr)
    {
     QListWidgetItem *item = new QListWidgetItem(QString::number(*itr),ui->clusters_listview);
     if(!delClusters->contains(*itr))
        item->setCheckState(Qt::Checked);
     else
        item->setCheckState(Qt::Unchecked);
     itemList->append(item);
    }

}

void MainWindow::add_to_list(QString &str)
{
    if (str.isEmpty() || abs(str.toInt()) > fileClusters)
        return;

    qDeleteAll(itemList->begin(),itemList->end());

    itemList->clear();
    ui->clusters_listview->clear();

    QList<unsigned int> clusters;
    if(str == "*")
    {
        for(int i=0; i <= fileClusters; ++i)
            clusters.append(i);
    }
    else
    {
        clusters= parse_clusters_str(str);
    }

    foreach (int num, clusters) {
        itemSet->insert(abs(num));
    }

    QList<int> numbers;
    for(auto itr=itemSet->begin(); itr!=itemSet->end(); ++itr)
    {
        numbers.push_back(*itr);
    }
    qSort(numbers.begin(),numbers.end());

    for(auto itr=numbers.begin(); itr!=numbers.end(); ++itr)
    {
     QListWidgetItem *item = new QListWidgetItem(QString::number(*itr),ui->clusters_listview);
     if(!delClusters->contains(*itr))
        item->setCheckState(Qt::Checked);
     else
        item->setCheckState(Qt::Unchecked);
     itemList->append(item);
    }

}
