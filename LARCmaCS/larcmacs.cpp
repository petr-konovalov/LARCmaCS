#include <math.h>
#include "larcmacs.h"
#include "ui_larcmacs.h"
#include "packetSSL.h"
#include "message.h"
#include <QFileDialog>
#include "settings.h"
#include "grSimRobot.h"

LARCmaCS::LARCmaCS(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::LARCmaCS),
	scalingRequested(true),
	sizescene(10),
	drawscale(1)
{
	ui->setupUi(this);
	fieldscene = new FieldScene();
	fieldscene->setReceiver(&receiver);
	ui->fieldView->setScene(fieldscene);
	scaleView(8);
	macsArray = new QString[12];

	receiver.init();
	mainalg.init(&receiver);
	sceneview.init();
	connector.init();

	//algorithm connect
	connect(this, SIGNAL(MLEvalString(QString)), &mainalg, SLOT(EvalString(QString)));
	//connect(this, SIGNAL(MatlabPause()), &mainalg.worker, SLOT(Pause()));

	//send command to robots
	connect(&mainalg, SIGNAL(sendToConnector(int, const QByteArray &)), &connector.worker, SLOT(run(int, const QByteArray &)));

	//gui connector
	connect(&sceneview.worker, SIGNAL(updateView()), this, SLOT(updateView()));
	connect(ui->sceneslider, SIGNAL(valueChanged(int)), this, SLOT(scaleView(int)));

	//info GUI
	connect(&mainalg, SIGNAL(UpdatePauseState(QString)), this, SLOT(UpdatePauseState(QString)));
	connect(&mainalg, SIGNAL(StatusMessage(QString)), this, SLOT(UpdateStatusBar(QString)));
	connect(&receiver, SIGNAL(UpdateSSLFPS(QString)), this, SLOT(UpdateSSLFPS(QString)));

	//remotecontrol
	connect(&remotecontol, SIGNAL(RC_control(int,int,int,int, bool)),
			this, SLOT(remcontrolsender(int, int,int, int, bool)));
	connect(this, SIGNAL(sendToConnectorRM(int, const QByteArray &)), &connector.worker, SLOT(run(int, const QByteArray &)));

	connect(this, SIGNAL(addIp(int, QString)), &connector.worker, SLOT(addIp(int, QString)));

	//simulator Enable
	connect(this, SIGNAL(ChangeSimulatorMode(bool)), &receiver, SLOT(ChangeSimulatorMode(bool)));
	connect(&receiver, SIGNAL(clearField()), fieldscene, SLOT(ClearField()));
	connect(this, SIGNAL(ChangeSimulatorMode(bool)), &mainalg, SLOT(setEnableSimFlag(bool)));
	connect(&mainalg, SIGNAL(sendToSimConnector(const QByteArray &)), &connector.worker, SLOT(runSim(const QByteArray &)));
	connect(this, SIGNAL(changeGrSimIP(const QString &)), &connector.worker, SLOT(changeGrSimIP(const QString &)));
	connect(this, SIGNAL(changeGrSimPort(unsigned short)), &connector.worker, SLOT(changeGrSimPort(unsigned short)));

	//ball inside check
	connect(&robotReceiver, SIGNAL(ballStatus(bool)), &mainalg, SLOT(changeBallStatus(bool)));

	sceneview.start();
	receiver.start();
	mainalg.start();
	connector.start();
	fieldscene->start();
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
		IP = connector.worker.getGrSimIP();
		port = connector.worker.getGrSimPort();
	}

	if(socket.ConnectedState == QUdpSocket::ConnectedState) {
		socket.writeDatagram(byteData, byteData.length(), QHostAddress(IP), port);
	} else {
		socket.connectToHost(IP, port);
		if (socket.ConnectedState == QUdpSocket::ConnectedState) {
			socket.writeDatagram(byteData, byteData.length(), QHostAddress(IP), port);
		}
	}

	return;
}

LARCmaCS::~LARCmaCS()
{
	delete ui;
}

void LARCmaCS::UpdatePauseState(QString message)
{
	ui->label_Pause->setText(message);
}

void LARCmaCS::UpdateSSLFPS(QString message)
{
	ui->label_SSL_FPS->setText(message);
}

void LARCmaCS::UpdateStatusBar(QString message)
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
	IpDialog *ipDialog = new IpDialog(connector.worker, this);
	ipDialog->setWindowModality(Qt::WindowModality::WindowModal);
	ipDialog->open();
	qDebug() << connector.worker.numIP;
}

void LARCmaCS::on_but_reference_clicked()
{
	Reference *refWindow = new Reference();
	refWindow->show();
}
