import QtQml 2.15
import QtQuick 2.15
import QtQuick.Layouts 1.15

Item {
	id: root

	property string extension: ""
	property bool animationEnabled: false

	property alias name: fileNameLabel.text
	property alias size: fileSizeLabel.text

	signal compressionRequested()

	Rectangle {
		id: background

		anchors.fill: parent
		color: "#F0EEED"

		RowLayout {
			id: content

			anchors.fill: parent

			Text {
				id: fileNameLabel

				Layout.preferredHeight: parent.height
				Layout.preferredWidth: parent.width - 200
				verticalAlignment: Text.AlignVCenter

				leftPadding: 10
				elide: Text.ElideRight
				color: "#030303"
				text: fileName
				font.pointSize: 12
			}

			Button {
				id: button

				Layout.preferredWidth: 100
				Layout.preferredHeight: 30
				Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

				text: root.extension === ".bmp" ? "Compress" : "Decompress"

				onButtonClicked: {
					root.compressionRequested();
					loading.visible = true;
				}

				Rectangle {
					id: loading

					anchors {
						horizontalCenter: parent.horizontalCenter
						verticalCenter: parent.verticalCenter
					}

					height: 10
					width: 10

					color: "#332C39"

					RotationAnimator {
						id: animator

						target: loading;
						from: 0;
						to: 360;
						duration: 1000
						running: loading.visible
						loops: Animation.Infinite
					}

					Binding {
						target: loading
						property: "visible"
						value: root.animationEnabled
					}
				}
			}

			Text {
				id: fileSizeLabel

				Layout.preferredHeight: parent.height
				Layout.alignment: Qt.AlignRight
				rightPadding: 10
				verticalAlignment: Text.AlignVCenter

				color: "#030303"
				font.pointSize: 12
				text: fileName
			}
		}
	}
}
