#include "worktime.h"
#include "ui_worktime.h"

worktime::worktime(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::worktime)
{
    ui->setupUi(this);
}

worktime::~worktime()
{
    delete ui;
}
void worktime::on_tableWidget_cellDoubleClicked(int row, int column){

}

