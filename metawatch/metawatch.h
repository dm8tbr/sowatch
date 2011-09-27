#ifndef METAWATCH_H
#define METAWATCH_H

#include <QtCore/QQueue>
#include <QtCore/QTimer>
#include <QtCore/QSettings>
#include <QtConnectivity/QBluetoothAddress>
#include <QtConnectivity/QBluetoothSocket>
#include <QtSystemInfo/QSystemAlignedTimer>
#include "watch.h"

using QTM_PREPEND_NAMESPACE(QBluetoothSocket);
using QTM_PREPEND_NAMESPACE(QBluetoothAddress);
using QTM_PREPEND_NAMESPACE(QSystemAlignedTimer);

namespace sowatch
{

class MetaWatchPaintEngine;

class MetaWatch : public Watch
{
    Q_OBJECT

public:
	explicit MetaWatch(const QBluetoothAddress& address, QSettings* settings = 0, QObject *parent = 0);
	~MetaWatch();

	static const int screenWidth = 96;
	static const int screenHeight = 96;
	static const int systemAreaHeight = 30;

	enum MessageType {
		NoMessage = 0,
		GetDeviceType = 0x01,
		GetDeviceTypeResponse = 0x02,
		GetInformationString = 0x03,
		GetInformationStringResponse = 0x04,
		AdvanceWatchHands = 0x20,
		SetVibrateMode = 0x23,
		SetRealTimeClock = 0x26,
		GetRealTimeClock = 0x27,
		GetRealTimeClockResponse = 0x28,
		StatusChangeEvent = 0x33,
		ButtonEvent = 0x34,
		WriteBuffer = 0x40,
		ConfigureMode = 0x41,
		ConfigureIdleBufferSize = 0x42,
		UpdateDisplay = 0x43,
		LoadTemplate = 0x44,
		EnableButton = 0x46,
		DisableButton = 0x47,
		ReadButtonConfiguration = 0x48,
		ReadButtonConfigurationResponse = 0x49,
		BatteryConfiguration = 0x53,
		LowBatteryWarning = 0x54,
		LowBatteryBluetoothOff = 0x55,
		ReadBatteryVoltage = 0x56,
		ReadBatteryVoltageResponse = 0x57,
		Accelerometer = 0xea
	};

	enum Mode {
		IdleMode = 0,
		ApplicationMode = 1,
		NotificationMode = 2
	};

	enum Button {
		BtnA = 0,
		BtnB,
		BtnC,
		BtnD,
		BtnE,
		BtnF
	};

	enum ButtonPress {
		PressOnly = 0,
		PressAndRelease = 1,
		HoldAndRelease = 2,
		LongHoldAndRelease = 3
	};

	QPaintEngine* paintEngine() const;
	int metric(PaintDeviceMetric metric) const;

	QString model() const;
	QStringList buttons() const;
	bool isConnected() const;
	bool busy() const;

	QDateTime dateTime();
	void setDateTime(const QDateTime& dateTime);

	void grabButton(int button);
	void ungrabButton(int button);

	void updateNotificationCount(Notification::Type type, int count);

	void displayIdleScreen();
	void displayNotification(Notification *notification);
	void displayApplication();

	void vibrate(int msecs);

	Mode currentMode() const;
	Mode paintTargetMode() const;
	QImage* imageFor(Mode mode);
	void clear(Mode mode, bool black = false);
	void update(Mode mode, const QList<QRect>& rects = QList<QRect>());
	void grabButton(Mode mode, Button button);
	void ungrabButton(Mode mode, Button button);

	void renderIdleScreen();
	void renderIdleWeather();
	void renderIdleCounts();
	void renderNotificationScreen();

	QImage iconForNotification(const Notification *n);

protected:
	// Some configurable stuff.
	bool _24hMode : 1;
	bool _dayMonthOrder : 1;
	bool _invertedIdle : 1;
	bool _invertedNotifications : 1;
	bool _invertedApplications : 1;
	short _notificationTimeout;

	// Notifications: Unread count
	uint _nMails, _nCalls, _nIms, _nSms, _nMms;

	// Notifications: timers
	QTimer* _idleTimer;
	QTimer* _ringTimer;

	// Buttons
	static const char btnToWatch[8];
	QStringList _buttonNames;

	// Current watch state
	Mode _currentMode;
	Mode _paintMode;

	// For QPaintDevice
	mutable MetaWatchPaintEngine* _paintEngine;
	QImage _image[3];

	// Timers to retry the connection when the watch is not found.
	static const int connectRetryTimesSize = 6;
	static const int connectRetryTimes[connectRetryTimesSize];
	short _connectRetries;
	bool _connected;
	QTimer* _connectTimer;
	QSystemAlignedTimer* _connectAlignedTimer;

	// Connection stuff
	QBluetoothAddress _address;
	QBluetoothSocket* _socket;

	// Base watch protocol stuff
	struct Message {
		MessageType type;
		quint8 options;
		QByteArray data;
		Message(MessageType ntype = NoMessage, QByteArray ndata = QByteArray(), quint8 noptions = 0) :
			type(ntype), options(noptions), data(ndata)
		{ }
	};

	QQueue<Message> _toSend;
	QTimer* _sendTimer;
	Message _partialReceived;

	static const quint8 bitRevTable[16];
	static const quint16 crcTable[256];
	quint16 calcCrc(const QByteArray& data, int size);
	quint16 calcCrc(const Message& msg);

	void send(const Message& msg);
	void handleMessage(const Message& msg);

	void setVibrateMode(bool enable, uint on, uint off, uint cycles);
	void updateLine(Mode mode, const QImage& image, int line);
	void updateLines(Mode mode, const QImage& image, int lineA, int lineB);
	void updateLines(Mode mode, const QImage& image, const QVector<bool>& lines);
	void configureWatchMode(Mode mode, int timeout, bool invert);
	void configureIdleSystemArea(bool entireScreen);
	void updateDisplay(Mode mode, bool copy = true);
	void loadTemplate(Mode mode, int templ);
	void enableButton(Mode mode, Button button, ButtonPress press);
	void disableButton(Mode mode, Button button, ButtonPress press);

	void handleStatusChange(const Message& msg);
	void handleButtonEvent(const Message& msg);

protected slots:
	void socketConnected();
	void socketDisconnected();
	void socketData();
	void socketError(QBluetoothSocket::SocketError error);
	void socketState(QBluetoothSocket::SocketState error);
	void retryConnect();
	void timedSend();
	void timedRing();

private:
	void realSend(const Message& msg);
	void realReceive(bool block);
};

}

#endif // METAWATCH_H
