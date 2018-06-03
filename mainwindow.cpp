
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    imgPath(new QString("/")),
    img_autoupdate(true),
    LISTVIEW_WIDTH(175),
    file_types(tr("JPEG File (*.jpg *.jpeg)")),
    itemSet(new QSet<unsigned int>),
    delClusters(new QSet<unsigned int>),
    itemList(new QList<QListWidgetItem*>),
    fileClusters(-1),
    status_lbl(new QLabel()),
    program("rundll32.exe C:\\WINDOWS\\System32\\shimgvw.dll,ImageView_Fullscreen ")
{
    ui->setupUi(this);
    setWindowTitle("JPEG Hero");
    ui->statusBar->addWidget(status_lbl);
    status_lbl->setText("No File Opened");
    ui->actionAutoupdate->setChecked(img_autoupdate);

    native_viewer = new QProcess(this);
    getWnd = new QProcess(this);

    QObject::connect(ui->actionAutoupdate,SIGNAL(triggered(bool)),this,SLOT(autoupdate()));
    QObject::connect(ui->action_Open,SIGNAL(triggered(bool)),this,SLOT(open_file()));
    QObject::connect(ui->action_Save,SIGNAL(triggered(bool)),this,SLOT(save_file()));
    QObject::connect(ui->actionSave_file_as,SIGNAL(triggered(bool)),this,SLOT(save_file_as()));
    QObject::connect(ui->actionRestore_default_file,SIGNAL(triggered(bool)),this,SLOT(resotre_file()));
    QObject::connect(ui->actionUpdate_image,SIGNAL(triggered(bool)),this,SLOT(update_file()));
    QObject::connect(ui->clusters_listview,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(cluster_clicked(QListWidgetItem*)));
    QObject::connect(this,SIGNAL(sig_update_file()),this,SLOT(update_file()));
    QObject::connect(this,SIGNAL(sig_update_view(int)),this,SLOT(update_view(int)));
    QObject::connect(ui->action_Add_Cluster, SIGNAL(triggered(bool)), this, SLOT(add_cluster()));
    QObject::connect(ui->actionRe_move_Cluster,SIGNAL(triggered(bool)),this,SLOT(remove_cluster()));
    QObject::connect(getWnd,SIGNAL(finished(int)),this,SLOT(update_view(int)));
    QObject::connect(ui->actionApply,SIGNAL(triggered(bool)),this,SLOT(find_delClusters()));

}

MainWindow::~MainWindow()
{
    if(QFile(tempFilePath).exists())
        QFile(tempFilePath).remove();
    if(QFile(backupPath).exists())
        QFile(backupPath).remove();

    delete ui,imgPath,imgView;
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
    clusters_list_orig = new QList<QByteArray>;

            while (!file.atEnd())
            {
                QByteArray cluster = file.read(CLUSTER_SIZE);
                clusters_list_orig->push_back(cluster);
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
               if (i<fileClusters)
                clusters.append(i);
       }
       else
           if(!re.exactMatch(substr) || substr == "")
               valid_num = false;
           if(!valid_num)
               continue;
           if (substr.toInt()<fileClusters)
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

    qDeleteAll(itemList->begin(),itemList->end());
    delClusters->clear();
    itemSet->clear();
    itemList->clear();
    ui->clusters_listview->clear();

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

    QString winPath = (tempFilePath).replace('/','\\');
    native_viewer->close();
    native_viewer->start(program + " " + winPath);
    getWnd->start("getWnd.exe");
}

void MainWindow::autoupdate()
{
    img_autoupdate = ui->actionAutoupdate->isChecked();
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

void MainWindow::update_view(int exitStatus)
{
    if(exitStatus>0)
        return;

    long wndId;
    QFile inputFile("viewer.wnd");
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          line.remove(0,2);
          wndId = line.toInt(nullptr,16);
       }
       inputFile.close();
    }

    nativeWindow = QWindow::fromWinId(wndId);
    nativeWindow->setFlags(Qt::FramelessWindowHint);
    imgView = QWidget::createWindowContainer(nativeWindow,this);
    ui->scrollArea->setWidget(imgView);
}

void MainWindow::update_file()
{
    if(QFile(tempFilePath).exists())
        QFile(tempFilePath).remove();

    QFile file(tempFilePath);

    if (!file.open(QIODevice::WriteOnly))
        return;

    for(int i=0; i<clusters_list_orig->size(); ++i)
    {   if(!delClusters->contains(i))
            file.write(clusters_list_orig->at(i));
    }

    file.close();

    QString winPath = (tempFilePath).replace('/','\\');
    native_viewer->close();
    native_viewer->start(program + " " + winPath);
    getWnd->start("getWnd.exe");
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

    QString winPath = (tempFilePath).replace('/','\\');
    native_viewer->close();
    native_viewer->start(program + " " + winPath);
    getWnd->start("getWnd.exe");

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
        for(int i=0; i < fileClusters; ++i)
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
        for(int i=0; i < fileClusters; ++i)
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



//НОВОЕ
int MainWindow::check_cluster(QByteArray &cluster, QList<QByteArray> *signatures_list) {
    int result; //результат
    int n; // число вхождений
    // считываем по одной сигнатуре и ищем ее в кластере
        n = 0;
        int current_result = 0;

        for (int i = 0; i < signatures_list->size(); i++) {
            for (int j = 0; j < cluster.size() - signatures_list->at(i).size(); j++) {
                int p = j;
                n = n + current_result;
                current_result = 1;
                for (int k = 0; k < signatures_list->at(i).size(); k++) {
                    if ((signatures_list->at(i).at(k) != cluster.at(p))&&(signatures_list->at(i).at(k) != '*')) {
                        current_result = 0;
                        break;
                    }
                    p = p + 1;
                }
            }
        }

    // если кол-во вхождений больше 19 или равно нулю, то кластер проверку не прошел
    if ((n > 19) || (n == 0)) {
        result = 0;
    }
    // в обратном случае - прошел
     else result = 1;

    return result;
}


QByteArray MainWindow::convert_to_bits(QByteArray &cluster) {
    QByteArray bit_cluster;
    for (int i = 0; i < cluster.size(); i++) {
        switch (cluster.at(i)) {
        case '0':
            bit_cluster.push_back("0000");
            break;
        case '1':
            bit_cluster.push_back("0001");
            break;
        case '2':
            bit_cluster.push_back("0010");
            break;
        case '3':
            bit_cluster.push_back("0011");
            break;
        case '4':
            bit_cluster.push_back("0100");
            break;
        case '5':
            bit_cluster.push_back("0101");
            break;
        case '6':
            bit_cluster.push_back("0110");
            break;
        case '7':
            bit_cluster.push_back("0111");
            break;
        case '8':
            bit_cluster.push_back("1000");
            break;
        case '9':
            bit_cluster.push_back("1001");
            break;
        case 'a':
            bit_cluster.push_back("1010");
            break;
        case 'b':
            bit_cluster.push_back("1011");
            break;
        case 'c':
            bit_cluster.push_back("1100");
            break;
        case 'd':
            bit_cluster.push_back("1101");
            break;
        case 'e':
            bit_cluster.push_back("1110");
            break;
        case 'f':
            bit_cluster.push_back("1111");
            break;
        }
    }
    return bit_cluster;
}

void MainWindow::find_delClusters() {

    int CLUSTER_SIZE = 4096;
    QFile file(tempFilePath);
    file.open(QIODevice::ReadOnly);

    clusters_list = new QList<QByteArray>;
        while (!file.atEnd())
        {
            QByteArray cluster = file.read(CLUSTER_SIZE);
            clusters_list->push_back(cluster.toHex());
        }
    file.close();

    clusters_list_bits = new QList<QByteArray>;


    for (int i = 0; i < clusters_list->size(); i++) {
        QByteArray new_cluster;
        new_cluster = clusters_list->at(i);
        new_cluster = convert_to_bits(new_cluster);
        clusters_list_bits->push_back(new_cluster);
    }
    printf("Hello");
    // открыли файл с сигнатурами
    signatures_list = new QList<QByteArray>;
        QFile sign_file(":/files/patterns.txt");
        sign_file.open(QIODevice::ReadOnly);
        while (!sign_file.atEnd()) {
                  QByteArray signature = sign_file.readLine();
                  // удаляем из сигнатуры пробел
                  signature.remove(signature.size()-1, 1);
                  signatures_list->push_back(signature);
              }
        sign_file.close();

    //delClusters = new QSet<unsigned int>; инициализация производится ОДИН!!!!! раз в конструкторе класса.
    //дальше работа идет с существующим объектом, если новая картинка, то просто удаляются все его эл-ты, НО не объект.

    #pragma omp parallel for
    for (int i = 0; i < clusters_list_bits->size(); i++){
        QByteArray test;
        test = clusters_list_bits->at(i);
        if (check_cluster(test, signatures_list) == 0) {
            delClusters->insert(i);
        }
    }

    QString update_all_cltrs_view = "*";
    this->add_to_list(update_all_cltrs_view);
    if (img_autoupdate)
    {
        emit sig_update_file();
    }
}
//
