import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: window
    color: "black"
    width: 400; height: 250

    signal sigCall(string strNumber)
    signal sigText(string strNumber)
    signal sigContacts
    signal sigInbox

    property string selectedOri: "UnknownOrientation"
    property variant activeOri: selectedOri == "UnknownOrientation" ? Code.getOriName(runtime.orientation) : selectedOri
    state: "orientation " + activeOri

    property bool landscape: window.width > window.height
    property variant rotationDelta: landscape? -90 : 0

    // initial state is portrait
    property real baseWidth: landscape ? window.height : window.width
    property real baseHeight: landscape ? window.width : window.height

    states: [
        State {
            name: "orientation " + "Portrait"
            PropertyChanges {
                target: main
                rotation: Code.getAngle("Portrait") + rotationDelta
                width: baseWidth
                height: baseHeight
            }
        },
        State {
            name: "orientation " + "Landscape"
            PropertyChanges {
                target: main
                rotation: Code.getAngle("Landscape") + rotationDelta
                width: baseHeight
                height: baseWidth
            }
        },
        State {
            name: "orientation " + "PortraitInverted"
            PropertyChanges {
                target: main
                rotation: Code.getAngle("PortraitInverted") + rotationDelta
                width: baseWidth
                height: baseHeight
            }
        },
        State {
            name: "orientation " + "LandscapeInverted"
            PropertyChanges {
                target: main
                rotation: Code.getAngle("LandscapeInverted") + rotationDelta
                width: baseHeight
                height: baseWidth
            }
        }
    ]

    Row {
        id: main

        anchors.fill: parent
        spacing: 2

        Column {
            width: parent.width / 2
            height: parent.height

            DialDisp {
                id: wDisp
                color: window.color

                width: parent.width
                height: parent.height * (4 / 5)
            }//DialDisp

            ActionButtons {
                id: btnSpecial
                width: parent.width
                height: window.height * (1 / 5)

                onSigCall: window.sigCall(wDisp.txtEd.text)
                onSigText: window.sigText(wDisp.txtEd.text)
                onSigContacts: window.sigContacts()
                onSigInbox: window.sigInbox()

                onSigDel: Code.doDel()
            }
        }

        Keypad {
            color: window.color

            width: parent.width / 2
            height: parent.height

            onBtnClick: {
                var origStart = wDisp.txtEd.selectionStart;
                var result = wDisp.txtEd.text.substr(0,origStart);
                result += strText;
                result += wDisp.txtEd.text.substr(wDisp.txtEd.selectionEnd);
                wDisp.txtEd.text = result;
                wDisp.txtEd.cursorPosition = origStart + strText.length;
            }

            onBtnDelClick: Code.doDel()
        }//Keypad

    }//Flow
}//Rectangle
