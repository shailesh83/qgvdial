import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: main
    width: 250; height: 400
    property bool landscape: main.width > main.height

    signal sigCall(string strNumber)
    signal sigText(string strNumber)

    MainButtons {
        onSigDialer: {
            console.debug("Clicked: Dialer")
        }
        onSigContacts: {
            console.debug("Clicked: Contacts")
        }
        onSigInbox: {
            console.debug("Clicked: Inbox")
        }
        onSigSettings: {
            console.debug("Clicked: Settings")
        }
    }//MainButton

    states: [
        State {
            name: "Dialer"
            PropertyChanges {
                target: name
            }
        }
    ]//states
}
