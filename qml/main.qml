import QtQml 2.15
import QtQuick 2.15
import QtQuick.Layouts

import FilesList 1.0

Window {
	id: root

	width: 600
	height: 400
	visible: true
	color: "#609EA2"
	title: qsTr("BMP Compressor")

	ColumnLayout {
		id: mainContent

		anchors {
			fill: parent
			margins: 10
		}

		FilesListView {
			id: filesListView

			Layout.preferredWidth: root.width - 100
			Layout.preferredHeight: 380
			Layout.alignment: Qt.AlignTop | Qt.AlignCenter

			model: fileslist
		}

		Timer {
			interval: 5000;
			running: true;
			repeat: true
			onTriggered: fileslist.updateFilesListRequested()
		}
	}
}
