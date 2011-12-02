import QtQuick 1.1
import com.nokia.meego 1.0

import "sowatch.js" as Sowatch

PageStackWindow {
    id: appWindow

    initialPage: mainPage

    MainPage {
        id: mainPage
    }

    ToolBarLayout {
        id: commonTools
        visible: true
        ToolIcon {
            platformIconId: "toolbar-view-menu"
            anchors.right: (parent === undefined) ? undefined : parent.right
            onClicked: (myMenu.status == DialogStatus.Closed) ? myMenu.open() : myMenu.close()
        }
    }

    Menu {
        id: myMenu
        visualParent: pageStack
        MenuLayout {
			MenuItem {
				text: qsTr("Start service")
				onClicked: {
					Sowatch.start();
				}
			}
			MenuItem {
				text: qsTr("Stop service")
				onClicked: {
					Sowatch.stop();
				}
			}
        }
    }
}