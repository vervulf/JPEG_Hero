
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
    itemList(new QList<QListWidgetItem*>)
{
    //QMessageBox::information(this, tr("Constructor"),tr("Constructor"));
    ui->setupUi(this);
    setWindowTitle("JPEG Hero");
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

    itemList->append(new QListWidgetItem("0", ui->clusters_listview));

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

}

MainWindow::~MainWindow()
{
    if(QFile(tempFilePath).exists())
        QFile(tempFilePath).remove();
    if(QFile(backupPath).exists())
        QFile(backupPath).remove();

    delete ui,img,imgPath,imgLabel;
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

    emit sig_update_view();
}

void MainWindow::resotre_file()
{
    delete img;
    img = new QPixmap(backupPath);

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

void MainWindow::cluster_clicked(QListWidgetItem *item)
{
    Qt::CheckState state = item->checkState();
    switch(state)
    {
    case Qt::Checked: item->setCheckState(Qt::Unchecked); break;
    case Qt::Unchecked: item->setCheckState(Qt::Checked); break;
    }

    if (img_autoupdate)
    {
        emit sig_update_file();
    }
    else
    {

    }

}
