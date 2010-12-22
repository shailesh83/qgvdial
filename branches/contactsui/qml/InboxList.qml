import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: container
    width: 250; height: 320
    color: "black"

    signal sigCall(string number)
    signal sigText(string number)

    Rectangle {
        id: detailsView

        anchors.fill: parent
        color: "darkslategray"
        border.color: "orange"
        radius: 10

        opacity: 0

        Item {
            id: detailTopRow

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            height: btnDetailsClose.height

            Text {
                text: "Contact name"    // contactDetailName
                anchors.verticalCenter: parent.verticalCenter
                color: "white"
                font.pointSize: Code.btnFontPoint () / 8
                anchors.left: parent.left
            }

            TextButton {
                id: btnDetailsClose
                text: "Close"
                onClicked: container.state= ''
                anchors.right: parent.right

                fontPoint: Code.btnFontPoint() / 8
            }
        }

        ListView {
            anchors {
                top: detailTopRow.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            clip: true

            model: contactDetailsModel
        }
    }

    ListView {
        id: contactsView

        anchors.fill: parent
        clip: true
        opacity: 1

        model: inboxModel

        delegate: Rectangle {
            id: listDelegate

            color: "darkslategray"
            border.color: "orange"
            radius: 10

            width: contactsView.width
            height: (contactsView.height / 5);

            Text {
                id: textName

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 5
                }

                text: name
                color: "white"

                font.pointSize: (Code.btnFontPoint () / 8)
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    container.sigContactlink (contacts);

//                    detailsView.model = contacts;

                    container.state = "Details"
                }
            }
        }// delegate Rectangle
    }// ListView

    states: [
        State {
            name: "Details"
            PropertyChanges { target: contactsView; opacity: 0 }
            PropertyChanges { target: detailsView; opacity: 1 }
        }
    ]

    transitions: [
        Transition {
            PropertyAnimation { property: "opacity"; easing.type: Easing.InOutQuad}
        }
    ]

}// Rectangle
