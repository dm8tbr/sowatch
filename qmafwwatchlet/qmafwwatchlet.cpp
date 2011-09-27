#include <MafwShared.h>

#include "qmafwwatchletplayer.h"
#include "qmafwwatchlet.h"

using namespace sowatch;

class WatchletPlayer : public QObject
{
	Q_OBJECT
};


QMafwWatchlet::QMafwWatchlet(WatchServer* server) :
	DeclarativeWatchlet(server, "com.javispedro.sowatch.qmafw"),
	_registry(MafwRegistry::instance()),
	_player(new QMafwWatchletPlayer(this))
{
	MafwShared* shared = MafwShared::instance();

	connect(_registry, SIGNAL(rendererAdded(QString, QString)), SLOT(handleRendererAdded(QString)));
	connect(_registry, SIGNAL(rendererRemoved(QString, QString)), SLOT(handleRendererRemoved(QString)));

	shared->initTracking(_registry);

	connect(this, SIGNAL(activated()), _player, SLOT(activate()));
	connect(this, SIGNAL(deactivated()), _player, SLOT(deactivate()));

	rootContext()->setContextProperty("player", _player);
	setSource(QUrl("qrc:/qmafwwatchlet/" + server->watch()->model() + ".qml"));
}

void QMafwWatchlet::handleRendererAdded(const QString &uuid)
{
	if (uuid == "mafw_gst_renderer") {
		_player->setRenderer(_registry->renderer(uuid));
	}
}

void QMafwWatchlet::handleRendererRemoved(const QString &uuid)
{
	if (uuid == "mafw_gst_renderer") {
		_player->setRenderer(0);
	}
}
