#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRect>
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    img(new QPixmap()),
    img_path(new QString()),
    scrollArea(new QScrollArea())
{
    int LISTVIEW_WIDTH = 175;
    ui->setupUi(this);
    setWindowTitle("JPEG Hero");

    ui->clusters_listview->setGeometry(0,0,0,0);
    ui->img_label->setGeometry(LISTVIEW_WIDTH,0,0,0);

    QDesktopWidget *dw = new QDesktopWidget();
    QRect *screenSize = new QRect();
    *screenSize = dw->availableGeometry(this);
    ui->img_label->resize(screenSize->width()-LISTVIEW_WIDTH,screenSize->height());
    ui->clusters_listview->resize(LISTVIEW_WIDTH,screenSize->height());

    QObject::connect(ui->action_Open,SIGNAL(triggered(bool)),this,SLOT(get_img()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::get_img()
{
    *img_path = QFileDialog::getOpenFileName(this,tr("Open File"),
                                             last_file_path, tr("JPEG File (*.jpg *.jpeg)"));
    if (*img_path == NULL)
        return;

    delete img;
    img = new QPixmap(*img_path);
    ui->img_label->setPixmap(*img);
}
