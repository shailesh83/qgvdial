import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: container
    height: (mainBtnList.model.count * btnReference.height) + mainBtnList.spacing

    width: parent.width
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter

    signal sigDialer
    signal sigContacts
    signal sigInbox
    signal sigSettings

    TextButton {
        id: btnReference
        opacity: 0
        text: "Contacts"
    }

    ListView {
        id: mainBtnList
        anchors.fill: parent

        model: ListModel {
            ListElement {
                name: "Dialer"
            }
            ListElement {
                name: "Contacts"
            }
            ListElement {
                name: "Inbox"
            }
            ListElement {
                name: "Settings"
            }
        }

        delegate: TextButton {
            id: btn
            text: name

            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: {
                if (name == "Dialer") {
                    container.sigDialer();
                } else if (name == "Contacts") {
                    container.sigContacts();
                } else if (name == "Inbox") {
                    container.sigInbox();
                } else if (name == "Settings") {
                    container.sigSettings();
                }
            }
        }
    }
}
