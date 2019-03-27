#include <math.h>
#include "larcmacs.h"
#include "ui_larcmacs.h"
#include "packetSSL.h"
#include "message.h"
#include <QFileDialog>
#include "settings.h"
#include "grsimtransforms.h"

LARCmaCS::LARCmaCS(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::LARCmaCS),
	scalingRequested(true),
	sizescene(10),
	drawscale(1)
{
	ui->setupUi(this);
	fieldscene = new FieldScene();
	ui->fieldView->setScene(fieldscene);
	scaleView(8);
	macsArray = new QString[12];

	receiver.init();
	mainalg.init();
	sceneview.init();
	connector.init();

//robots connect
//    connect(&wifiform,SIGNAL(initRobots()),&connector.worker, SLOT(startBroadcast()));
//    connect(&wifiform,SIGNAL(stopInit()),&connector.worker, SLOT(stopBroadcast()));
//    connect(&connector.worker,SIGNAL(robotAdded(QString)),&wifiform,SLOT(addRobot(QString)));
//    connect(&connector.worker,SIGNAL(allNeededRobotsEnabled()),&wifiform,SLOT(initEnded()));

	// GUIS
	connect(&connector.worker,SIGNAL(sendPortList(QStringList)),this,SLOT(displayPorts(QStringList)));
	connect(this,SIGNAL(openPort(QString)),&connector.worker,SLOT(openPort(QString)));

	//algorithm connect
	connect(this, SIGNAL(MLEvalString(QString)),&mainalg.worker,SLOT(EvalString(QString)));
	//connect(this, SIGNAL(MatlabPause()), &mainalg.worker, SLOT(Pause()));
	connect(&receiver.worker, SIGNAL(activateMA(PacketSSL)), &mainalg.worker, SLOT(run(PacketSSL)));
	connect(&mainalg.worker, SIGNAL(mainAlgFree()), &receiver.worker, SLOT(MainAlgFree()));
	//reciever
	connect(this,SIGNAL(ChangeMaxPacketFrequencyMod(bool)),&receiver.worker,SLOT(ChangeMaxPacketFrequencyMod(bool)));

	//send command to robots
//    connect(this,SIGNAL(receiveMacArray(QString*)),&connector.worker,SLOT(receiveMacArray(QString*)));
	connect(&mainalg.worker, SIGNAL(sendToConnector(int, const QByteArray &)), &connector.worker, SLOT(run(int, const QByteArray &)));

	//gui connector
	connect(&sceneview.worker, SIGNAL(updateView()), this, SLOT(updateView()));
	connect(ui->sceneslider, SIGNAL(valueChanged(int)), this, SLOT(scaleView(int)));

	//info GUI
	connect(&mainalg.worker,SIGNAL(UpdatePauseState(QString)),this,SLOT(UpdatePauseState(QString)));
	connect(&mainalg.worker, SIGNAL(StatusMessage(QString)), this, SLOT(UpdateStatusBar(QString)));
	connect(&receiver.worker, SIGNAL(UpdateSSLFPS(QString)), this, SLOT(UpdateSSLFPS(QString)));

	//remotecontrol
	connect(&remotecontol,SIGNAL(RC_control(int,int,int,int, bool)),
			this,SLOT(remcontrolsender(int, int,int, int, bool)));
	connect(this,SIGNAL(sendToConnectorRM(int, const QByteArray &)),&connector.worker,SLOT(run(int, const QByteArray &)));

	QObject::connect(this, SIGNAL(addIp(int, QString)), &connector.worker, SLOT(addIp(int, QString)));

	//simulator Enable
	connect(this, SIGNAL(ChangeSimulatorMode(bool)), &receiver.worker, SLOT(ChangeSimulatorMode(bool)));
	connect(&receiver.worker, SIGNAL(clearField()), this, SLOT(clearUIField()));
	connect(this, SIGNAL(ChangeSimulatorMode(bool)), &mainalg.worker, SLOT(setEnableSimFlag(bool)));
	connect(&mainalg.worker, SIGNAL(sendToSimConnector(const QByteArray &)), &connector.worker, SLOT(runSim(const QByteArray &)));
	connect(this, SIGNAL(changeGrSimIP(const QString &)), &connector.worker, SLOT(changeGrSimIP(const QString &)));
	connect(this, SIGNAL(changeGrSimPort(unsigned short)), &connector.worker, SLOT(changeGrSimPort(unsigned short)));

	//fieldScene Update
	connect(&receiver.worker,SIGNAL(activateGUI(SSL_WrapperPacket *)),this, SLOT(fieldsceneUpdateRobots(SSL_WrapperPacket *)));
	connect(&receiver.worker, SIGNAL(updatefieldGeometry(SSL_WrapperPacket *)), this, SLOT (fieldsceneUpdateField(SSL_WrapperPacket *)));
	connect(this,SIGNAL(updateRobots()),fieldscene,SLOT(update()));
	connect(this, SIGNAL(updateGeometry()),fieldscene,SLOT(update()));
	//    connect(&receiver.worker, SIGNAL(activateGUI(PacketSSL)), &sceneview.worker, SLOT(repaintScene(PacketSSL)));

	connect(&robotReceiver, SIGNAL(ballStatus(bool)), &mainalg.worker, SLOT(changeBallStatus(bool)));

	sceneview.start();
	receiver.start();
	mainalg.start();
	connector.start();
	UpdateStatusBar("Waiting SSL connection...");
	UpdateSSLFPS("FPS=0");
}

void LARCmaCS::remcontrolsender(int l, int r,int k, int b, bool kickUp)
{
	QString ip = ui->lineEditRobotIp->text();
	QByteArray byteData;
	bool simFlag = mainalg.worker.getIsSimEnabledFlag();
	if (!simFlag) {
		Message data;
		data.setSpeedX(l);
		data.setSpeedY(r);
		data.setSpeedR(k);
		data.setSpeedDribbler(0);
		data.setDribblerEnable(0);

		if(b != -1) {
			data.setKickVoltageLevel(4);
			data.setKickerChargeEnable(1);
			data.setKickUp(kickUp);
			data.setKickForward(0);
		} else {
			data.setKickVoltageLevel(0);
			data.setKickerChargeEnable(0);
			data.setKickUp(0);
			data.setKickForward(0);
		}

		byteData = data.generateByteArray();
	} else {
		int numOfRobot = ip.toInt();
		GrSimTransforms::formGrSimControlPacket(byteData, numOfRobot, r, l, k, kickUp, 0, 4, 0);
	}

	unsigned short port;
	QString IP;
	if (!simFlag) {
		IP = ip;
		port = REAL_ROBOT_PORT;
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

void LARCmaCS::fieldsceneUpdateRobots(SSL_WrapperPacket * packet)
{
	fieldscene->UpdateRobots(packet);
	emit updateRobots();
}

void LARCmaCS::fieldsceneUpdateField(SSL_WrapperPacket * packet)
{
#ifdef OLD_SSL_PROTO
	fieldscene->UpdateGeometry(receiver.worker.fieldsize);
	emit updateGeometry();
#else
	fieldscene->UpdateField(packet);
	emit updateRobots();
#endif
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
//    cout << _sizescene << "  " << sizescene;
//    qreal scaleFactor = (drawScale-1) - (qreal)_scaleFactor/100;
//    cout << scaleFactor << "  ";
//    qreal factor = ui->view->matrix().scale ( scaleFactor, scaleFactor ).mapRect ( QRectF ( 0, 0, 1, 1 ) ).width();
//    cout << factor << "  ";
//    if ( factor > 0.07 && factor < 100.0 )
//    drawscale = 1 - (float)(sizescene-_sizescene)/10;
	drawscale = pow(0.9, _sizescene-sizescene);
	sizescene = _sizescene;
//    ui->view->wheelEvent();
//    cout << " DS " << drawscale << endl;
	scalingRequested = true;
}

void LARCmaCS::updateView()
{
//  static float lastScale = 0;
//  if ( shutdownSoccerView )
//  {
//    return;
//  }
	if (scalingRequested) {
		qreal factor = ui->fieldView->matrix().scale (drawscale, drawscale).mapRect (QRectF ( 0, 0, 1, 1 )).width();
		if (factor > 0.07 && factor < 100.0)
			ui->fieldView->scale (drawscale, drawscale);
		scalingRequested = false;
		ui->fieldView->viewport()->update();
	}
}

void LARCmaCS::clearUIField()
{
	fieldscene->ClearField();
	emit updateRobots();
}

void LARCmaCS::on_pushButton_SetMLdir_clicked()
{
	Settings settings;
	static const char * key = "MLScriptsDir";
	QString  dir = settings.value(key, QCoreApplication::applicationDirPath()).toString();
	dir = QFileDialog::getExistingDirectory(Q_NULLPTR, QString(), dir);
	if (!dir.isEmpty()) {
		settings.setValue(key, dir);
		QString  s = "cd "+ dir;
		qDebug() << "New Matlab directory = " << s;
		emit MLEvalString(s);
	}
}

void LARCmaCS::on_pushButton_RC_clicked()
{
	remotecontol.hide();
	remotecontol.show();
	remotecontol.TimerStart();
}

void LARCmaCS::on_checkBox_MlMaxFreq_stateChanged(int state)
{
	emit(ChangeMaxPacketFrequencyMod(state > 0));
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
