#include <QtDebug>

#include "watchesmodel.h"

using namespace sowatch;

WatchesModel::WatchesModel(QObject *parent) :
    QAbstractListModel(parent),
    _config(new GConfKey("/apps/sowatch", this)),
    _watches_list(_config->getSubkey("watches", this)),
    _daemon(new DaemonProxy("com.javispedro.sowatchd", "/com/javispedro/sowatch/daemon", QDBusConnection::sessionBus()))
{
	QHash<int, QByteArray> roles = roleNames();
	roles[Qt::DisplayRole] = QByteArray("title");
	roles[Qt::StatusTipRole] = QByteArray("subtitle");
	roles[EnabledRole] = QByteArray("enabled");
	roles[ConfigKeyRole] = QByteArray("configKey");
	roles[ConfigQmlUrlRole] = QByteArray("configQmlUrl");
	setRoleNames(roles);

	connect(_watches_list, SIGNAL(changed()),
	        SLOT(handleWatchesListChanged()));
	connect(_daemon, SIGNAL(WatchStatusChanged(QString,QString)),
	        this, SLOT(handleWatchStatusChanged(QString,QString)));

	reload();
}

WatchesModel::~WatchesModel()
{
}

int WatchesModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return _list.count();
}

QVariant WatchesModel::data(const QModelIndex &index, int role) const
{
	ConfigKey *config = _list[index.row()];
	QString key = config->key();
	QString id = key.mid(key.lastIndexOf('/') + 1);
	switch (role) {
	case Qt::DisplayRole:
		return config->value("name");
	case Qt::StatusTipRole:
		if (config->value("enable").toBool()) {
			QString status = _status[id];
			if (status == "connected") {
				return QVariant(tr("Connected"));
			} else if (status == "enabled") {
				return QVariant(tr("Disconnected, Searching..."));
			} else {
				return QVariant(tr("Disconnected"));
			}
		} else {
			return QVariant(tr("Disabled"));
		}
	case EnabledRole:
		return config->value("enable");
	case ConfigKeyRole:
		return QVariant::fromValue(key);
	case ConfigQmlUrlRole:
		if (config->isSet("driver")) {
			QString driver = config->value("driver").toString();
			WatchPluginInterface *plugin = Registry::registry()->getWatchPlugin(driver);
			if (plugin) {
				return QVariant::fromValue(plugin->getConfigQmlUrl(driver));
			}
		}
		return QVariant::fromValue(QUrl());
	}
	return QVariant();
}

bool WatchesModel::removeRows(int row, int count, const QModelIndex &parent)
{
	Q_UNUSED(row);
	Q_UNUSED(count);
	Q_UNUSED(parent);
	return false; // TODO
}

void WatchesModel::addFoundWatch(const QVariantMap &info)
{
	QStringList existing = _config->dirs();
	QString base = "watch%1";
	QString name = base.arg("");
	int num = 1;

	while (existing.contains(name)) {
		num++;
		name = base.arg(num);
	}

	// Load the autodetected settings
	ConfigKey* newkey = _config->getSubkey(name);
	foreach (const QString& key, info.keys()) {
		newkey->set(key, info[key]);
	}

	// Set some defaults
	Registry *registry = Registry::registry();
	foreach (const QString& providerId, registry->allNotificationProviders()) {
		qDebug() << "Would add" << providerId;
	}

	// Now add to the watches list
	QStringList active = _watches_list->value().toStringList();
	active << name;
	_watches_list->set(active);

	// Now enable
	newkey->set("enable", true);
}

void WatchesModel::reload()
{
	QStringList names = _watches_list->value().toStringList();

	beginResetModel();
	foreach (ConfigKey* conf, _list) {
		delete conf;
	}
	_status.clear();
	_list.clear();
	foreach (const QString& s, names) {
		_list.append(_config->getSubkey(s, this));
		QDBusReply<QString> reply = _daemon->GetWatchStatus(s);
		if (reply.isValid()) {
			_status[s] = reply.value();
		}
	}
	endResetModel();

	qDebug() << "Found" << _list.count() << "configured watches";
}

void WatchesModel::handleWatchesListChanged()
{
	reload();
}

void WatchesModel::handleWatchStatusChanged(const QString &watch, const QString &status)
{
	_status[watch] = status;
	int i = findRowByWatchId(watch);
	if (i != -1) {
		emit dataChanged(createIndex(i, 0), createIndex(i, 0));
	}
}

int WatchesModel::findRowByWatchId(const QString &id)
{
	QString pattern(_config->key() + "/" + id);
	for (int i = 0; i < _list.size(); i++) {
		if (_list[i]->key().endsWith("/" + id)) {
			return i;
		}
	}
	return -1;
}
