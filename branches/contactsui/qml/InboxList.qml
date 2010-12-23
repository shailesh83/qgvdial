import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: container
    width: 250; height: 320
    color: "black"

    signal sigCall(string number)
    signal sigText(string number)
    signal sigVoicemail(string link)
    signal sigInboxSelect(string selection)

    // Private properties. DO NOT TOUCH from outside.
    property string strTopRow: ""
    property bool isVoicemail: false
    property string theNumber: ""
    property string strSelected: "All"

    Rectangle { // Details
        id: detailsView

        anchors.fill: parent
        color: "darkslategray"
        border.color: "orange"
        radius: 10

        opacity: 0

        Item {  // Top row
            id: detailTopRow

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
            }
            height: btnDetailsClose.height

            Text {
                text: strTopRow
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

        Item {  // Number and buttons
            anchors {
                top: detailTopRow.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            Text {
                text: theNumber
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                }
                color: "white"
                font.pointSize: Code.btnFontPoint () / 8
            }

            Row {
                anchors {
                    right: parent.right
                }

                TextButton {
                    text: "Call"
                    onClicked: container.sigCall(theNumber)
                }
                TextButton {
                    text: "Text"
                    onClicked: container.sigText(theNumber)
                }
                TextButton {
                    text: "Listen"
                    opacity: (opacity & isVoicemail)
                    onClicked: container.sigListen()
                }
            }
        }
    }

    Item { //  The combined inbox list and selector list
        id: inboxView
        anchors.fill: parent
        opacity: 1

        Rectangle { // Selector bar at the top
            id: barTop
            width: parent.width
            height: parent.height / 15
            anchors.top: parent.top

            color: "black"

            signal clickedTopBar

            Text {
                text: strSelected
                font.pointSize: Code.btnFontPoint()/10
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
            }

            MouseArea {
                id: mouseAreaTopBar
                anchors.fill: parent

                onClicked: {
                    barTop.clickedTopBar();
                    inboxView.state = 'Selectors';
                }
            }// MouseArea

            states: [
                State {
                    name: "pressed"
                    when: mouseAreaTopBar.pressed
                    PropertyChanges { target: barTop; color: "orange" }
                }
            ]
        }// Rectangle (selector)

        ListView {
            id: listSelector
            anchors {
                top: barTop.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            opacity: 0
            clip: true

            model: ["All", "Placed", "Missed", "Received", "Voicemail", "SMS"]
            delegate:  Rectangle {
                height: lblSelector.height
                width: listSelector.width

                color: "black"
                border.color: "orange"

                Text {
                    id: lblSelector
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData
                    font.pointSize: Code.btnFontPoint()/12
                    color: "white"
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        inboxView.state = '';
                        strSelected = modelData
                        container.sigInboxSelect(modelData)
                    }
                }
            }// TextButton
        }

        ListView {
            id: listInbox
            anchors {
                top: barTop.bottom
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            opacity: 1

            clip: true

            model: inboxModel

            delegate: Rectangle {
                id: listDelegate

                color: "darkslategray"
                border.color: "orange"
                radius: 2

                width: inboxView.width
                height: textName.height + 6

                Text {
                    id: textName

                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        leftMargin: 5
                    }

                    text: type + " " + time + " : " + name

                    color: "white"

                    font.pointSize: (Code.btnFontPoint () / 12)
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        strTopRow = type + " at " + time + " : " + name;

                        if (type == "Voicemail") {
                            isVoicemail = true;
                        } else {
                            isVoicemail = false;
                        }

                        container.state = "Details"
                    }
                }
            }// delegate Rectangle
        }// ListView

        states: [
            State {
                name: "Selectors"
                PropertyChanges { target: listInbox; opacity: 0 }
                PropertyChanges { target: listSelector; opacity: 1 }
            }
        ]

        transitions: [
            Transition {
                PropertyAnimation { property: "opacity"; easing.type: Easing.InOutQuad}
            }
        ]
    }// Item (list and selector)

    states: [
        State {
            name: "Details"
            PropertyChanges { target: inboxView; opacity: 0 }
            PropertyChanges { target: detailsView; opacity: 1 }
        }
    ]

    transitions: [
        Transition {
            PropertyAnimation { property: "opacity"; easing.type: Easing.InOutQuad}
        }
    ]

}// Rectangle
