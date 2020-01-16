#include "orionoverlaywidget.h"
#include "ui_orionoverlaywidget.h"

OverlayWidget::OverlayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OverlayWidget)
{
    ui->setupUi(this);

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(execute_repeat()));
    timer->start(1);
    ui->backmain->hide();
}

OverlayWidget::~OverlayWidget()
{
    delete ui;
}

void OverlayWidget::on_Calibration_clicked()
{

}

void OverlayWidget::execute_repeat()
{

}

void OverlayWidget::on_Connection_clicked()
{
    ui->Mirror->hide();
    ui->Cali_info->hide();
    ui->Connection->hide();
    ui->Calibration->hide();

    ui->backmain->show();
    isconn = true;
}

void OverlayWidget::on_backmain_clicked()
{
    ui->Mirror->show();
    ui->Cali_info->show();
    ui->Connection->show();
    ui->Calibration->show();

    ui->backmain->hide();
    isconn = false;
}
