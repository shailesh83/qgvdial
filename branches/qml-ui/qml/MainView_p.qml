import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: window
    color: "black"
    width: 250; height: 400

    signal sigCall(string strNumber)
    signal sigText(string strNumber)
    signal sigContacts
    signal sigInbox

    property bool landscape: window.width > window.height

    // initial state is portrait
    property real baseWidth: landscape ? window.height : window.width
    property real baseHeight: landscape ? window.width : window.height

    Rectangle {
        color: "white"

        width: baseWidth
        height: baseHeight
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        rotation: (landscape?-90:0)

        Column {
            anchors.fill: parent
            width: baseWidth
            height: baseHeight

            DialDisp {
                id: wDisp
                color: window.color

                width: parent.width
                height: parent.height * (4 / 9)
            }//DialDisp

            Keypad {
                color: window.color

                width: parent.width
                height: parent.height * (4 / 9)

                onBtnClick: {
                }

                onBtnDelClick: Code.doDel()
            }//Keypad

            ActionButtons {
                color: window.color

                width: parent.width
                height: parent.height * (1 / 9)

                onSigCall: window.sigCall(wDisp.txtEd.text)
                onSigText: window.sigText(wDisp.txtEd.text)
                onSigContacts: window.sigContacts()
                onSigInbox: window.sigInbox()

                onSigDel: Code.doDel()
            }
        }//Column
    }//Rectangle
}//Rectangle
