#ifndef PATIENTINFORMATION_H
#define PATIENTINFORMATION_H

#include <QDialog>

namespace Ui {
class patientInformation;
}

class patientInformation : public QDialog
{
    Q_OBJECT

public:
    explicit patientInformation(QWidget *parent = nullptr);
    ~patientInformation();

private:
    Ui::patientInformation *ui;


private slots:
    void on_btnExit1_clicked();
    void on_btnMin_clicked();
};

#endif // PATIENTINFORMATION_H
