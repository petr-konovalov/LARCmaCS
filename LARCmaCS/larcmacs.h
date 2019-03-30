#pragma once

#include <QWidget>
#include "fieldScene.h"
#include "receiver.h"
#include "mainAlg.h"
#include "sceneView.h"
#include "connector.h"
#include "remotecontrol.h"
#include "ipdialog.h"
#include "reference.h"
#include "robotReceiver.h"

#define NUM_CONTROL_ROBOTS 12

namespace Ui
{
	class LARCmaCS;
}

class LARCmaCS : public QWidget
{
	Q_OBJECT

public:
	RemoteControl remotecontol;
	explicit LARCmaCS(QWidget *parent = 0);
	~LARCmaCS();

	FieldScene *fieldscene;
	QString * macsArray;

protected:
	bool scalingRequested;
	SceneView sceneview;
	Receiver receiver;
	MainAlg mainalg;
	Connector connector;

private:
	Ui::LARCmaCS *ui;
	float drawscale;
	qreal sizescene;
	QString wifiaddrdata[NUM_CONTROL_ROBOTS];
	QUdpSocket socket;
	RobotReceiver robotReceiver;

private slots:
	void fieldsceneUpdateRobots(SSL_WrapperPacket * packet);
	void fieldsceneUpdateField(SSL_WrapperPacket * packet);
	void UpdateSSLFPS(QString message);
	void UpdateStatusBar(QString message);
	void UpdatePauseState(QString message);
	void clearUIField();
	void updateView();
	void scaleView(int);

private slots:
	//void on_pushButton_Pause_clicked();
	void on_pushButton_SetMLdir_clicked();
	void on_pushButton_RC_clicked();
	void remcontrolsender(int l, int r,int k, int b, bool kickUp);
	void on_checkBox_MlMaxFreq_stateChanged(int state);
	void on_checkBox_SimEnable_stateChanged(int state);
	void on_pushButton_RemoteControl_clicked();
	void on_pushButton_SetupIP_clicked();
	void on_but_reference_clicked();

signals:
	void sendToConnectorRM(int N,QByteArray command);
	void changeGrSimIP(QString IP);
	void changeGrSimPort(unsigned short port);
	void ChangeSimulatorMode(bool state);
	void receiveMacArray(QString*);
	void ChangeMaxPacketFrequencyMod(bool state);
	//void MatlabPause();
	void MLEvalString(QString s);
	void updateRobots();
	void openPort(QString);
	void addIp(int, QString);
};
