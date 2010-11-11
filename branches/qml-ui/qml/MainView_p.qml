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

    property variant selectedOri: Orientation.UnknownOrientation
    property variant activeOri: selectedOri == Orientation.UnknownOrientation ? runtime.orientation : selectedOri
    state: "orientation " + activeOri

    property bool landscape: window.width > window.height
    property variant rotationDelta: landscape? -90 : 0

    // initial state is portrait
    property real baseWidth: landscape ? window.height : window.width
    property real baseHeight: landscape ? window.width : window.height

    states: [
        State {
            name: "orientation " + Orientation.Landscape
            PropertyChanges {
                target: main
                rotation: Code.getAngle(Orientation.Landscape) + rotationDelta
                width: baseHeight
                height: baseWidth
            }
        },
        State {
            name: "orientation " + Orientation.PortraitInverted
            PropertyChanges {
                target: main
                rotation: Code.getAngle(Orientation.PortraitInverted) + rotationDelta
                width: baseWidth
                height: baseHeight
            }
        },
        State {
            name: "orientation " + Orientation.LandscapeInverted
            PropertyChanges {
                target: main
                rotation: Code.getAngle(Orientation.LandscapeInverted) + rotationDelta
                width: baseHeight
                height: baseWidth
            }
        }
    ]

    Flow {
        id: main

        anchors.fill: parent
        spacing: 2

        DialDisp {
            id: wDisp
            color: window.color

            width: window.width
            height: window.height * (4 / 9)
        }//DialDisp

        Keypad {
            color: window.color

            width: window.width
            height: window.height * (4 / 9)

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

        ActionButtons {
            id: btnSpecial
            width: window.width
            height: window.height * (1 / 9)

            onSigCall: window.sigCall(wDisp.txtEd.text)
            onSigText: window.sigText(wDisp.txtEd.text)
            onSigContacts: window.sigContacts()
            onSigInbox: window.sigInbox()

            onSigDel: Code.doDel()
        }
    }//Flow
}//Rectangle
