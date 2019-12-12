#ifndef WORKTIME_H
#define WORKTIME_H

#include <QDialog>

namespace Ui {
class worktime;
}

class worktime : public QDialog
{
    Q_OBJECT

public:
    explicit worktime(QWidget *parent = nullptr);
    ~worktime();

private slots:
    void on_tableWidget_cellDoubleClicked(int row, int column);

private:
    Ui::worktime *ui;
};

#endif // WORKTIME_H
