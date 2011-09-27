#ifndef SOWATCH_WATCHLET_H
#define SOWATCH_WATCHLET_H

#include <QtCore/QObject>
#include "sowatch_global.h"

namespace sowatch
{

class Watch;
class WatchServer;

class SOWATCH_EXPORT Watchlet : public QObject
{
    Q_OBJECT
	Q_PROPERTY(QString id READ id CONSTANT)
	Q_PROPERTY(bool isActive READ isActive NOTIFY activeChanged)

public:
	explicit Watchlet(WatchServer *server, const QString& id);
	virtual ~Watchlet();

	WatchServer* server();
	Watch* watch();

	Q_INVOKABLE QString id() const;
	bool isActive() const;

signals:
	void activeChanged();
	void activated();
	void deactivated();

protected:
	virtual void activate();
	virtual void deactivate();

	const QString _id;
	bool _active;

private:
	WatchServer* _server;

friend class WatchServer;
};

}

#endif // SOWATCH_WATCHLET_H
