#include "watchscannermodel.h"

WatchScannerModel::WatchScannerModel(QObject *parent) :
    QAbstractListModel(parent),
    _scanner(new sowatch::AllWatchScanner(this)),
    _timer(new QTimer(this)),
    _enabled(false), _active(false)
{
	QHash<int, QByteArray> roles = roleNames();
	roles[Qt::DisplayRole] = QByteArray("title");
	roles[Qt::StatusTipRole] = QByteArray("subtitle");
	roles[ObjectRole] = QByteArray("object");
	setRoleNames(roles);

	_timer->setSingleShot(true);
	_timer->setInterval(3000);

	connect(_scanner, SIGNAL(watchFound(QVariantMap)), SLOT(handleWatchFound(QVariantMap)));
	connect(_scanner, SIGNAL(started()), SLOT(handleStarted()));
	connect(_scanner, SIGNAL(finished()), SLOT(handleFinished()));
	connect(_timer, SIGNAL(timeout()), SLOT(handleTimeout()));
}

WatchScannerModel::~WatchScannerModel()
{
}

bool WatchScannerModel::enabled() const
{
	return _enabled;
}

void WatchScannerModel::setEnabled(bool enabled)
{
	_timer->stop();

	_enabled = enabled;

	if (_enabled && !_active) {
		_scanner->start();
	}
}

bool WatchScannerModel::active() const
{
	return _active;
}

int WatchScannerModel::rowCount(const QModelIndex &parent) const
{
	return _list.count();
}

QVariant WatchScannerModel::data(const QModelIndex &index, int role) const
{
	qDebug() << "Asked for data" << index.row() << index.column() << role;
	const QVariantMap &info = _list.at(index.row());
	switch (role) {
	case Qt::DisplayRole:
		return info["name"];
	case Qt::StatusTipRole:
		return info["address"];
	case ObjectRole:
		return QVariant::fromValue(info);
	}
	return QVariant();
}

void WatchScannerModel::handleWatchFound(const QVariantMap &info)
{
	qDebug() << "Watch found" << info << endl;
	if (!_list.contains(info)) {
		int count = _list.count();
		beginInsertRows(QModelIndex(), count, count);
		_list.append(info);
		endInsertRows();
	}
}

void WatchScannerModel::handleStarted()
{
	_active = true;
	emit activeChanged();
}

void WatchScannerModel::handleFinished()
{
	qDebug() << "Scan finished";
	_active = false;
	if (_enabled) {
		_timer->start();
	}
	emit activeChanged();
}

void WatchScannerModel::handleTimeout()
{
	qDebug() << "Restarting scan";
	_scanner->start();
}