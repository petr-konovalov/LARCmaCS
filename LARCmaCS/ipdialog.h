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
	explicit IpDialog(SharedRes * sharedRes, QWidget *parent = nullptr);
	~IpDialog();

private slots:
	void on_buttonBox_accepted();
	void on_buttonBox_rejected();

private:
	Ui::IpDialog * ui;
	SharedRes * mSharedRes;
};
