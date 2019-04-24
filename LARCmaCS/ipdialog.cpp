#include "ipdialog.h"
#include "ui_ipdialog.h"
#include "settings.h"

IpDialog::IpDialog(const QMap<int, QString> & ipList,  QWidget * parent) :
	QDialog(parent),
	ui(new Ui::IpDialog)
{
	ui->setupUi(this);
	ui->lineEditIp1->setText(ipList[1]);
	ui->lineEditIp2->setText(ipList[2]);
	ui->lineEditIp3->setText(ipList[3]);
	ui->lineEditIp4->setText(ipList[4]);
	ui->lineEditIp5->setText(ipList[5]);
	ui->lineEditIp6->setText(ipList[6]);
	ui->lineEditIp7->setText(ipList[7]);
	ui->lineEditIp8->setText(ipList[8]);
	ui->lineEditIp9->setText(ipList[9]);
	ui->lineEditIp10->setText(ipList[10]);
	ui->lineEditIp11->setText(ipList[11]);
	ui->lineEditIp12->setText(ipList[12]);
}

IpDialog::~IpDialog()
{
	delete ui;
}

void IpDialog::on_buttonBox_accepted()
{
	emit(addIp(1, ui->lineEditIp1->text()));
	emit(addIp(2, ui->lineEditIp2->text()));
	emit(addIp(3, ui->lineEditIp3->text()));
	emit(addIp(4, ui->lineEditIp4->text()));
	emit(addIp(5, ui->lineEditIp5->text()));
	emit(addIp(6, ui->lineEditIp6->text()));
	emit(addIp(7, ui->lineEditIp7->text()));
	emit(addIp(8, ui->lineEditIp8->text()));
	emit(addIp(9, ui->lineEditIp9->text()));
	emit(addIp(10, ui->lineEditIp10->text()));
	emit(addIp(11, ui->lineEditIp11->text()));
	emit(addIp(12, ui->lineEditIp12->text()));
	this->close();
}

void IpDialog::on_buttonBox_rejected()
{
	this->close();
}
