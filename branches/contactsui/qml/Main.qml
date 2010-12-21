import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: main
    width: 250; height: 400

    signal sigCall(string strNumber)
    signal sigText(string strNumber)

    property bool landscape: (main.width > main.height)
    property string strStatus: "Ready"


    Item {
        id: mainColumn
        anchors.fill: parent
        property int centralHeight: parent.height - barTop.height - barStatus.height
        property int centralWidth: parent.width

        Rectangle {
            id: barTop
            width: parent.width
            height: parent.height / 15
            anchors.top: parent.top

            color: "black"

            signal clickedTopBar

            Text {
                text: "qgvdial"
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
                    main.state = '';
                }
            }// MouseArea

            states: [
                State {
                    name: "pressed"
                    when: mouseAreaTopBar.pressed
                    PropertyChanges { target: barTop; color: "orange" }
                }
            ]
        }

////////////////////////////////////////////////////////////////////////////////
//                          Co-existent Items Begin                           //
////////////////////////////////////////////////////////////////////////////////
        Rectangle {
            id: mainRect

            width: mainColumn.centralWidth
            height: mainColumn.centralHeight
            anchors.top: barTop.bottom
            anchors.bottom: barStatus.top

            color: "black"
            opacity: 1

            MainButtons {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                onSigDialer:    { main.state = "Dialer" }
                onSigContacts:  { main.state = "Contacts" }
                onSigInbox:     { main.state = "Inbox" }
                onSigSettings:  { main.state = "Settings" }
            }//MainButton
        }

        MainView {
            id: dialer

            width: mainColumn.centralWidth
            height: mainColumn.centralHeight
            anchors.top: barTop.bottom
            anchors.bottom: barStatus.top

            opacity: 0
        }

        ContactsList {
            id: contactsList

            width: mainColumn.centralWidth
            height: mainColumn.centralHeight
            anchors.top: barTop.bottom
            anchors.bottom: barStatus.top

            opacity: 0
        }

        InboxList {
            id: inboxList

            width: mainColumn.centralWidth
            height: mainColumn.centralHeight
            anchors.top: barTop.bottom
            anchors.bottom: barStatus.top

            opacity: 0
        }

////////////////////////////////////////////////////////////////////////////////
//                           Co-existent Items End                            //
////////////////////////////////////////////////////////////////////////////////

        Rectangle {
            id: barStatus
            width: parent.width
            height: parent.height / 20
            anchors.bottom: parent.bottom

            color: "black"

            Text {
                text: main.strStatus
                font.pointSize: Code.btnFontPoint()/12
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
            }
        }
    }//Column

    states: [
        State {
            name: "Dialer"
            PropertyChanges { target: dialer; opacity: 1}
        },
        State {
            name: "Contacts"
            PropertyChanges { target: contactsList; opacity: 1}
        },
        State {
            name: "Inbox"
            PropertyChanges { target: inboxList; opacity: 1}
        },
        State {
            name: "Settings"
        }
    ]//states

    transitions: [
        Transition {
            PropertyAnimation { property: "opacity"; easing.type: Easing.InOutQuad}
            PropertyAnimation { target: dialer; property: "opacity"; easing.type: Easing.InOutQuad}
        }
    ]
}
