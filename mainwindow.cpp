
#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    img(new QPixmap()),
    img_path(new QString("C://")),
    scrollArea(new QScrollArea()),
    img_fits_wnd(true),
    img_autoupdate(false),
    LISTVIEW_WIDTH(175)
{
    //QMessageBox::information(this, tr("Constructor"),tr("Constructor"));
    ui->setupUi(this);
    setWindowTitle("JPEG Hero");
    ui->actionAutoupdate->setChecked(img_autoupdate);
    ui->actionImage_fits_window->setChecked(img_fits_wnd);

    QSize *wndSize = new QSize();
    *wndSize = this->size();

    int imgW = wndSize->width()-LISTVIEW_WIDTH,
        imgH = wndSize->height()-77, // 77 = bottom status bar height
        clustW = LISTVIEW_WIDTH,
        clustH = wndSize->height()-77;

    ui->clusters_listview->setGeometry(0,0,clustW,clustH);
    ui->img_label->setGeometry(LISTVIEW_WIDTH,0,imgW,imgH);

    QObject::connect(ui->action_Open,SIGNAL(triggered(bool)),this,SLOT(get_img()));
    QObject::connect(ui->actionAutoupdate,SIGNAL(triggered(bool)),this,SLOT(autoupdate()));
    QObject::connect(ui->actionImage_fits_window,SIGNAL(triggered(bool)),this,SLOT(fit_size()));

}

MainWindow::~MainWindow()
{
    delete ui,img,img_path,scrollArea;
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Resize)
        QMessageBox::information(this, tr("Resize"), tr("RESIZE"));
}


void MainWindow::get_img()
{
    QString file_types = tr("JPEG File (*.jpg *.jpeg)");
    *img_path = QFileDialog::getOpenFileName(this,tr("Open File"),
                                             *img_path, file_types);
    if (*img_path == NULL)
    {
        *img_path = "C://";
        return;
    }

    delete img;
    img = new QPixmap(*img_path);
    if (img_fits_wnd)
    {
        int w = ui->img_label->width(),
            h = ui->img_label->height();
        ui->img_label->setPixmap(img->scaled(w,h,Qt::KeepAspectRatio));
    }
    else
    {
        int w = img->width(),
            h = img->height();
        ui->img_label->resize(w,h);
        ui->img_label->setPixmap(*img);
    }
}

void MainWindow::fit_size()
{
    img_fits_wnd = ui->actionImage_fits_window->isChecked();
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

    ui->img_label->setGeometry(LISTVIEW_WIDTH,0,imgW,imgH);
}

void MainWindow::save_file()
{

}

void MainWindow::save_file_as()
{

}
