import Qt 4.7
import "../../trunk/qml/helper.js" as Code

Rectangle {
    width: 400; height: 250
    color: "black"

    ListModel {
        id: contactsModel

        ListElement {
            name: "Uv"
            contacts: [
                ListElement {
                    type: "Mobile"
                    number: "+1 408 905 9884"
                },
                ListElement {
                    type: "Home"
                    number: "+1 408 916 5616"
                }
            ]
        }
        ListElement {
            name: "Yasho"
            contacts: [
                ListElement {
                    type: "Mobile"
                    number: "+1 408 905 9883"
                },
                ListElement {
                    type: "Work"
                    number: "+1 408 567 5885"
                }
            ]
        }
    }

    ListView {
        id: listView
        model: contactsModel
        anchors.fill: parent

        delegate: Item {
            id: listDelegate
            property real detailsOpacity: 0

            width: listView.width
            height: {
                if (detailsOpacity == 0) {
                    return (listView.height / 5);
                } else {
                    return listView.height;
                }
            }

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    if (listDelegate.detailsOpacity == 0) {
                        listDelegate.detailsOpacity = 1;
                    } else {
                        listDelegate.detailsOpacity = 0;
                    }
                }
            }

            // Bounding rectangle
            Rectangle {
                anchors.fill: parent
                color: "darkslategray"
                border.color: "orange"
                radius: 10
                opacity: 1
            }

            Text {
                id: nameText
                anchors {
                    top: parent.top
                    left: parent.left
                    topMargin: 10
                    leftMargin: 3
                }

                text: (listDelegate.detailsOpacity==0 ? model.name : model.name + " details")
                color: "white"

                font.pointSize: (Code.btnFontPoint () / 8)
            }

            Item {
                id: smallForm
                opacity: (1 - listDelegate.detailsOpacity)
                anchors.fill: parent
            }

            Item {
                id: bigForm
                opacity: listDelegate.detailsOpacity
                anchors {
                    top: nameText.bottom
                    left: parent.left
                }

                width: parent.width
                height: (parent.height - nameText.height)

                ListView {
                    id: listPhones
                    model: contacts

                    delegate: Rectangle {
                        width: listPhones.width
                        height: (listPhones.height / 2)

                        Text {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            text: "woo" // listPhones.model.type
                            color: "white"
                            font.pointSize: (Code.btnFontPoint () / 8)
                        }
                        Text {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            text: number
                            color: "white"
                            font.pointSize: (Code.btnFontPoint () / 8)
                        }
                    }
                }
            }
        }// delegate Rectangle
    }// ListView
}// Rectangle
