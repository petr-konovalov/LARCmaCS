#include "ipdialog.h"
#include "ui_ipdialog.h"
#include "settings.h"

IpDialog::IpDialog(SharedRes * sharedRes,  QWidget * parent) :
	QDialog(parent),
	ui(new Ui::IpDialog)
{
	mSharedRes = sharedRes;
	ui->setupUi(this);
	ui->lineEditIp1->setText(sharedRes->getRobotIP(0));
	ui->lineEditIp2->setText(sharedRes->getRobotIP(1));
	ui->lineEditIp3->setText(sharedRes->getRobotIP(2));
	ui->lineEditIp4->setText(sharedRes->getRobotIP(3));
	ui->lineEditIp5->setText(sharedRes->getRobotIP(4));
	ui->lineEditIp6->setText(sharedRes->getRobotIP(5));
	ui->lineEditIp7->setText(sharedRes->getRobotIP(6));
	ui->lineEditIp8->setText(sharedRes->getRobotIP(7));
	ui->lineEditIp9->setText(sharedRes->getRobotIP(8));
	ui->lineEditIp10->setText(sharedRes->getRobotIP(9));
	ui->lineEditIp11->setText(sharedRes->getRobotIP(10));
	ui->lineEditIp12->setText(sharedRes->getRobotIP(11));
}

IpDialog::~IpDialog()
{
	delete ui;
}

void IpDialog::on_buttonBox_accepted()
{
	mSharedRes->setRobotIP(1, ui->lineEditIp1->text());
	mSharedRes->setRobotIP(2, ui->lineEditIp2->text());
	mSharedRes->setRobotIP(3, ui->lineEditIp3->text());
	mSharedRes->setRobotIP(4, ui->lineEditIp4->text());
	mSharedRes->setRobotIP(5, ui->lineEditIp5->text());
	mSharedRes->setRobotIP(6, ui->lineEditIp6->text());
	mSharedRes->setRobotIP(7, ui->lineEditIp7->text());
	mSharedRes->setRobotIP(8, ui->lineEditIp8->text());
	mSharedRes->setRobotIP(9, ui->lineEditIp9->text());
	mSharedRes->setRobotIP(10, ui->lineEditIp10->text());
	mSharedRes->setRobotIP(11, ui->lineEditIp11->text());
	mSharedRes->setRobotIP(12, ui->lineEditIp12->text());
	this->close();
}

void IpDialog::on_buttonBox_rejected()
{
	this->close();
}
