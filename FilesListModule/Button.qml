import QtQuick 2.15
import QtQuick.Layouts 1.15

MouseArea {
	id: root

	property alias text: buttonName.text
	signal buttonClicked()

	hoverEnabled: true

	onClicked: root.buttonClicked()

	Rectangle {
		id: background

		anchors.fill: parent
		color: root.containsMouse ? "#9c2556" : "#C92C6D"
		radius: 4

		Text {
			id: buttonName

			anchors {
				horizontalCenter: parent.horizontalCenter
				verticalCenter: parent.verticalCenter
			}

			color: "#F0EEED"

			font.pointSize: 12
		}
	}
}
