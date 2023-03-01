import QtQuick 2.15
import QtQuick.Layouts 1.15

Item {
	id: root

	property QtObject model: null
	signal buttonClicked()

	ColumnLayout {
		anchors.fill: parent
		spacing: 0

		Rectangle {
			id: listHeader

			Layout.fillWidth: true
			Layout.preferredHeight: 20

			color: "#332C39"

			RowLayout {
				id: content

				anchors.fill: parent

				Text {
					id: fileNameLabel

					Layout.preferredWidth: 100
					Layout.preferredHeight: parent.height
					leftPadding: 10
					verticalAlignment: Text.AlignVCenter

					color: "#F0EEED"
					text: "FILE NAME"
					font.pointSize: 8
				}

				Text {
					id: fileSizeLabel

					Layout.preferredHeight: parent.height
					Layout.alignment: Qt.AlignRight
					rightPadding: 40
					verticalAlignment: Text.AlignVCenter

					color: "#F0EEED"
					text: "SIZE"
					font.pointSize: 8
				}
			}
		}

		Rectangle {
			Layout.fillWidth: true
			Layout.fillHeight: true

			clip: true
			color: "#609EA2"

			ListView {
				id: filesList

				anchors.fill: parent
				model: root.model

				spacing: 10

				delegate: FilesListViewDelegate {
					id: delegate

					height: 40
					width: root.width

					name: fileName
					size: fileSize + " KB"
					extension: fileExtension

					onCompressionRequested: {
						if (fileExtension === ".bmp") {
							root.model.requestCompression(index);
						} else {
							root.model.requestDecompression(index);
						}
					}

					Connections {
						target: root.model
						function onCompressionFinished() {
							delegate.animationEnabled = false;
						}
					}

					Connections {
						target: root.model
						function onDecompressionFinished() {
							delegate.animationEnabled = false;
						}
					}
				}
			}
		}
	}
}
