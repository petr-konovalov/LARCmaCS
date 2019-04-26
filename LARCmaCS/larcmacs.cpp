#include <math.h>
#include "larcmacs.h"
#include "ui_larcmacs.h"
#include "packetSSL.h"
#include "message.h"
#include <QFileDialog>
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
{
	ui->setupUi(this);
	ui->fieldView->setScene(fieldscene);
	scaleView(8);
	macsArray = new QString[Constants::maxNumOfRobots];

	sceneview.init();
	connector.init(&sharedRes);

	//algorithm connect
	connect(this, SIGNAL(MLEvalString(const QString &)), &mainalg, SLOT(EvalString(const QString &)));
	//connect(this, SIGNAL(MatlabPause()), &mainalg.worker, SLOT(Pause()));

	//send command to robots
	connect(&mainalg, SIGNAL(sendToConnector(int, const QByteArray &)), &connector, SLOT(run(int, const QByteArray &)));

	//gui connector
	connect(&sceneview.worker, SIGNAL(updateView()), this, SLOT(updateView()));
	connect(ui->sceneslider, SIGNAL(valueChanged(int)), this, SLOT(scaleView(int)));

	//info GUI
	connect(&mainalg, SIGNAL(UpdatePauseState(const QString &)), this, SLOT(UpdatePauseState(const QString &)));
	connect(&mainalg, SIGNAL(StatusMessage(const QString &)), this, SLOT(UpdateStatusBar(const QString &)));
	connect(&receiver, SIGNAL(updateSSLFPS(const QString &)), this, SLOT(UpdateSSLFPS(const QString &)));

	//remotecontrol
	connect(&remotecontol, SIGNAL(RC_control(int, int, int, int, bool)),
			this, SLOT(remcontrolsender(int, int, int, int, bool)));

	//simulator Enable
	connect(this, SIGNAL(ChangeSimulatorMode(bool)), &receiver, SLOT(changeSimulatorMode(bool)));
	connect(&receiver, SIGNAL(clearField()), fieldscene, SLOT(ClearField()));
	connect(this, SIGNAL(ChangeSimulatorMode(bool)), &mainalg, SLOT(setEnableSimFlag(bool)));
	connect(&mainalg, SIGNAL(sendToSimConnector(const QByteArray &)), &connector, SLOT(runSim(const QByteArray &)));
	connect(this, SIGNAL(changeGrSimIP(const QString &)), &connector, SLOT(changeGrSimIP(const QString &)));
	connect(this, SIGNAL(changeGrSimPort(unsigned short)), &connector, SLOT(changeGrSimPort(unsigned short)));

	sceneview.start();
	connector.start();
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
		port = Connector::robotPort;
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

void LARCmaCS::on_checkBox_SimEnable_stateChanged(int state)
{
	if (state > 0) {
		emit changeGrSimIP(ui->lineEditSimIP->text());
		emit changeGrSimPort(ui->lineEditSimPort->text().toInt());
	}
	emit ChangeSimulatorMode(state > 0);
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
