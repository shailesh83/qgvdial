import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: main
    width: 250; height: 400

    signal sigCall(string strNumber)
    signal sigText(string strNumber)

    Rectangle {
        color: "black"
        anchors.fill: parent
        opacity: (main.state == ''? 1 : 0)

        MainButtons {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter

            onSigDialer: {
                console.debug("Clicked: Dialer")
//                main.state = "Dialer"
            }
            onSigContacts: {
                console.debug("Clicked: Contacts")
//                main.state = "Contacts"
            }
            onSigInbox: {
                console.debug("Clicked: Inbox")
//                main.state = "Inbox"
            }
            onSigSettings: {
                console.debug("Clicked: Settings")
//                main.state = "Settings"
            }
        }//MainButton
    }

//    MainView {
//        anchors.fill: parent
//        opacity: (main.state == "Dialer" ? 1 : 0)
//    }

    states: [
        State {
            name: "Dialer"
        },
        State {
            name: "Contacts"
        },
        State {
            name: "Inbox"
        },
        State {
            name: "Settings"
        }
    ]//states
}
