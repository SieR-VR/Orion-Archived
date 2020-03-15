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

    RedPalette.setBrush(QPalette::WindowText, QBrush(QColor(255,0,0)));
    GreenPalette.setBrush(QPalette::WindowText,QBrush(QColor(0,255,0)));

    ui->btnbackmain->hide();
    ui->ConnectionWidget->hide();

    ui->Chest_Connection->setPalette(RedPalette);
    ui->Waist_Connection->setPalette(RedPalette);
    ui->RT_Connection->setPalette(RedPalette);
    ui->LT_Connection->setPalette(RedPalette);
    ui->RC_Connection->setPalette(RedPalette);
    ui->LC_Connection->setPalette(RedPalette);
    ui->RF_Connection->setPalette(RedPalette);
    ui->LF_Connection->setPalette(RedPalette);
    ui->btnbackmain->hide();
}

OverlayWidget::~OverlayWidget()
{
    delete ui;
}

void OverlayWidget::execute_repeat()
{

    if(pStruct->connecting[CHEST])
        ui->Chest_Connection->setPalette(GreenPalette);
    else
        ui->Chest_Connection->setPalette(RedPalette);
    if(pStruct->connecting[WAIST])
        ui->Waist_Connection->setPalette(GreenPalette);
    else
        ui->Waist_Connection->setPalette(RedPalette);
    if(pStruct->connecting[RIGHT_THIGH])
        ui->RT_Connection->setPalette(GreenPalette);
    else
        ui->RT_Connection->setPalette(RedPalette);
    if(pStruct->connecting[LEFT_THIGH])
        ui->LT_Connection->setPalette(GreenPalette);
    else
        ui->LT_Connection->setPalette(RedPalette);
    if(pStruct->connecting[RIGHT_CALF])
        ui->RC_Connection->setPalette(GreenPalette);
    else
        ui->RC_Connection->setPalette(RedPalette);
    if(pStruct->connecting[LEFT_CALF])
        ui->LC_Connection->setPalette(GreenPalette);
    else
        ui->LC_Connection->setPalette(RedPalette);
    if(pStruct->connecting[RIGHT_FOOT])
        ui->RF_Connection->setPalette(GreenPalette);
    else
        ui->RF_Connection->setPalette(RedPalette);
    if(pStruct->connecting[LEFT_FOOT])
        ui->LF_Connection->setPalette(GreenPalette);
    else
        ui->LF_Connection->setPalette(RedPalette);

}


void OverlayWidget::on_btnConnection_clicked()
{

    ui->title->setText("Connection Management");

    ui->btnbackmain->show();
    ui->mainScreenWidget->hide();
    ui->ConnectionWidget->show();
}

void OverlayWidget::on_btnbackmain_clicked()
{

    ui->title->setText("Orion Settings");

    ui->btnbackmain->hide();
    ui->ConnectionWidget->hide();
    ui->mainScreenWidget->show();
}

void OverlayWidget::on_btnQuit_clicked()
{
   QApplication::quit();
}
