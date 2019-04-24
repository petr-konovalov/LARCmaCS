#pragma once

#include <QDialog>
#include "connector.h"
#include "receiver.h"

namespace Ui {
	class IpDialog;
}

class IpDialog : public QDialog
{
	Q_OBJECT

public:
	explicit IpDialog(const QMap<int, QString> & ipList, QWidget *parent = nullptr);
	~IpDialog();

private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();

private:
	Ui::IpDialog * ui;

signals:
	void addIp(int, const QString &);
};
