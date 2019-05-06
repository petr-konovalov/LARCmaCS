#pragma once
#include "engineInterface.h"
#include "mlData.h"
#include <QSharedPointer>
#include "packetSSL.h"

class MatlabEngine : public EngineInterface
{
public:
	MatlabEngine(SharedRes * sharedRes);
	void evaluate();
	virtual void pauseUnpause();
	virtual void setDirectory(const QString & path);

private:
	void runMatlab();
	void evalString(const QString & str);
	QSharedPointer<PacketSSL> MatlabEngine::loadVisionData();
	void processPacket(const QSharedPointer<PacketSSL> & packetssl);
	void updatePauseState();

	char mMatlabOutputBuffer[Constants::matlabOutputBufferSize];
	MlData mMatlabData;
	bool fmtlab;
};
