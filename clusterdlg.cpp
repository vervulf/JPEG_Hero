#include "clusterdlg.h"
#include "ui_clusterdlg.h"

ClusterDlg::ClusterDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ClusterDlg)
{
    ui->setupUi(this);
    setWindowTitle("Enter clusters");
    QObject::connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(okClicked()));
}

ClusterDlg::~ClusterDlg()
{
    delete ui;
}

void ClusterDlg::okClicked()
{
    QString str = ui->lineEdit->text();
    emit send_string(str);
    this->close();
}
