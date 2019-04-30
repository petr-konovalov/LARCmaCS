#include "larcmacs.h"

#include <QFileDialog>
#include <QMenu>
#include <math.h>

#include "ui_larcmacs.h"
#include "packetSSL.h"
#include "message.h"
#include "settings.h"
#include "grSimRobot.h"

LARCmaCS::LARCmaCS(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::LARCmaCS)
	, scalingRequested(true)
	, sizescene(10)
	, drawscale(1)
	, receiver(&sharedRes)
	, robotReceiver(&sharedRes)
	, fieldscene(new FieldScene(&sharedRes))
	, mainalg(&sharedRes)
	, connector(&sharedRes)
{
	ui->setupUi(this);
	ui->fieldView->setScene(fieldscene);
	scaleView(8);
	macsArray = new QString[Constants::maxNumOfRobots];

	sceneview.init();

	//algorithm connect
	connect(this, SIGNAL(MLEvalString(const QString &)), &mainalg, SLOT(EvalString(const QString &)));
	connect(this, SIGNAL(updateMatlabDebugFrequency(int)), &mainalg, SIGNAL(updateMatlabDebugFrequency(int)));
	//connect(this, SIGNAL(MatlabPause()), &mainalg.worker, SLOT(Pause()));

	connect(ui->matlabConsole, SIGNAL(customContextMenuRequested(const QPoint &)),
			this, SLOT(matlabConsoleMenuRequested(const QPoint &)));

	//send command to robots
	connect(&mainalg, SIGNAL(sendToConnector(int, const QByteArray &)), &connector, SLOT(run(int, const QByteArray &)));

	//gui connector
	connect(&sceneview.worker, SIGNAL(updateView()), this, SLOT(updateView()));
	connect(ui->sceneslider, SIGNAL(valueChanged(int)), this, SLOT(scaleView(int)));

	connect(&mainalg, SIGNAL(toMatlabConsole(const QString &)), this, SLOT(toMatlabConsole(const QString &)));

	//info GUI
	connect(&mainalg, SIGNAL(UpdatePauseState(const QString &)), this, SLOT(UpdatePauseState(const QString &)));
	connect(&mainalg, SIGNAL(engineStatistics(const QString &)), this, SLOT(UpdateStatusBar(const QString &)));
	connect(&receiver, SIGNAL(updateSSLFPS(const QString &)), this, SLOT(UpdateSSLFPS(const QString &)));

	//remotecontrol
	connect(&remotecontol, SIGNAL(RC_control(int, int, int, int, bool)),
			this, SLOT(remcontrolsender(int, int, int, int, bool)));

	//simulator Enable
	connect(this, SIGNAL(connectorChanged(bool, const QString &, int))
				, &receiver, SLOT(changeSimulatorMode(bool, const QString &, int)));
	connect(&receiver, SIGNAL(clearField()), fieldscene, SLOT(ClearField()));
	connect(&mainalg, SIGNAL(sendToSimConnector(const QByteArray &)), &connector, SLOT(runSim(const QByteArray &)));

	connect(this, SIGNAL(connectorChanged(bool, const QString &, int))
				, &connector, SLOT(onConnectorChange(bool, const QString &, int)));

	connect(this, SIGNAL(connectorChanged(bool, const QString &, int))
				, &mainalg, SIGNAL(connectorChanged(bool, const QString &, int)));

	sceneview.start();
	UpdateStatusBar("Waiting SSL connection...");
	UpdateSSLFPS("FPS = 0");
}

void LARCmaCS::remcontrolsender(int l, int r,int k, int b, bool kickUp)
{
	QString ip = ui->lineEditRobotIp->text();
	QByteArray byteData;
	bool simFlag = mainalg.getIsSimEnabledFlag();
	if (!simFlag) {
		DefaultRobot::formControlPacket(byteData, 0, r, l, k, kickUp, 0, 4, 0);
	} else {
		int numOfRobot = ip.toInt();
		GrSimRobot::formControlPacket(byteData, numOfRobot, r, l, k, kickUp, 0, 4, 0);
	}

	unsigned short port;
	QString IP;
	if (!simFlag) {
		IP = ip;
		port = connector.getRobotPort();
	} else {
		IP = connector.getGrSimIP();
		port = connector.getGrSimPort();
	}

	if(socket.ConnectedState == QUdpSocket::ConnectedState) {
		socket.writeDatagram(byteData, byteData.length(), QHostAddress(IP), port);
	} else {
		socket.connectToHost(IP, port);
		if (socket.ConnectedState == QUdpSocket::ConnectedState) {
			socket.writeDatagram(byteData, byteData.length(), QHostAddress(IP), port);
		}
	}
}

LARCmaCS::~LARCmaCS()
{
	delete ui;
}

void LARCmaCS::UpdatePauseState(const QString & message)
{
	ui->label_Pause->setText(message);
}

void LARCmaCS::UpdateSSLFPS(const QString & message)
{
	ui->label_SSL_FPS->setText(message);
}

void LARCmaCS::UpdateStatusBar(const QString & message)
{
	ui->StatusLabel->setText(message);
}

void LARCmaCS::scaleView(int _sizescene)
{
	drawscale = pow(0.9, _sizescene - sizescene);
	sizescene = _sizescene;
	scalingRequested = true;
}

void LARCmaCS::toMatlabConsole(const QString & str)
{
	ui->matlabConsole->appendPlainText(str);
}

void LARCmaCS::updateView()
{
	if (scalingRequested) {
		qreal factor = ui->fieldView->matrix().scale (drawscale, drawscale).mapRect (QRectF(0, 0, 1, 1)).width();
		if (factor > 0.07 && factor < 100.0)
			ui->fieldView->scale (drawscale, drawscale);
		scalingRequested = false;
		ui->fieldView->viewport()->update();
	}
}

void LARCmaCS::on_pushButton_SetMLdir_clicked()
{
	Settings settings;
	static const char * key = "MLScriptsDir";
	QString  dir = settings.value(key, QCoreApplication::applicationDirPath()).toString();
	dir = QFileDialog::getExistingDirectory(Q_NULLPTR, QString(), dir);
	if (!dir.isEmpty()) {
		settings.setValue(key, dir);
		QString s = "cd " + dir;
		qDebug() << "New Matlab directory = " << s;
		emit MLEvalString(s);
	}
}

void LARCmaCS::matlabConsoleMenuRequested(const QPoint & point)
{
	QMenu * menu = new QMenu(this);
	QAction * editDevice = new QAction(trUtf8("Clear console"), this);
	connect(editDevice, SIGNAL(triggered()), ui->matlabConsole, SLOT(clear()));
	menu->addAction(editDevice);
	menu->popup(ui->matlabConsole->viewport()->mapToGlobal(point));
}

void LARCmaCS::on_matlabOutputFrequencyLineEdit_textEdited(const QString & text)
{
	bool isInt = false;
	int frequency = text.split(" ")[0].toInt(&isInt);
	if (isInt && frequency != 0) {
		emit updateMatlabDebugFrequency(frequency);
	}
}

void LARCmaCS::on_checkBox_SimEnable_stateChanged(int state)
{
	emit connectorChanged(state > 0, ui->lineEditSimIP->text(), ui->lineEditSimPort->text().toInt());
}

void LARCmaCS::on_pushButton_RemoteControl_clicked()
{
	remotecontol.hide();
	remotecontol.show();
	remotecontol.TimerStart();
}

void LARCmaCS::on_pushButton_SetupIP_clicked()
{
	IpDialog * ipDialog = new IpDialog(&sharedRes, this);
	ipDialog->setWindowModality(Qt::WindowModality::WindowModal);
	ipDialog->open();
}

void LARCmaCS::on_but_reference_clicked()
{
	Reference * refWindow = new Reference();
	refWindow->show();
}
