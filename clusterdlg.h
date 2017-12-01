#ifndef CLUSTERDLG_H
#define CLUSTERDLG_H

#include <QDialog>
#include "include_libs.h"

namespace Ui {
class ClusterDlg;
}

class ClusterDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ClusterDlg(QWidget *parent = 0);
    ~ClusterDlg();

private:
    Ui::ClusterDlg *ui;

private slots:
    void okClicked();

signals:
    void send_string(QString&);
};

#endif // CLUSTERDLG_H
