#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"

#include <sowatch.h>

#include "watchesmodel.h"
#include "scanwatchesmodel.h"

static sowatch::Registry *registry;
static WatchesModel *watches;
static ScanWatchesModel *watchScanner;

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    QScopedPointer<QmlApplicationViewer> viewer(QmlApplicationViewer::create());

	registry = sowatch::Registry::registry();
	watches = new WatchesModel(app.data());
	watchScanner = new ScanWatchesModel(app.data());

	viewer->rootContext()->setContextProperty("watches", watches);
	viewer->rootContext()->setContextProperty("watchScanner", watchScanner);

    viewer->setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
	viewer->setMainQmlFile(QLatin1String("qml/main.qml"));
    viewer->showExpanded();

    return app->exec();
}
