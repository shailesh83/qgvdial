import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: window
    color: "black"
//    width: 250; height: 400
    width: 400; height: 250

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
//        anchors.fill: parent
        width: baseWidth
        height: baseHeight

//        rotation: (landscape?-90:0)
        rotation: {
            if (landscape) {
                console.debug ("-90");
                return -90;
            } else {
                console.debug ("0");
                return 0;
            }
        }

        Column {
            anchors.fill: parent

            DialDisp {
                id: wDisp
                color: window.color

                width: {
                    var w = parent.width;
                    var h = parent.height;

                    console.debug ("w="+w+",h="+h);

                    if (!landscape) {
                        console.debug ("p");
                        return (w);
                    } else {
                        console.debug ("l");
                        return (h);
                    }
                }

                height: (landscape?parent.width:parent.height) * (4 / 9)
            }//DialDisp

            Keypad {
                color: window.color

                width: (!landscape?parent.width:parent.height)
                height: (landscape?parent.width:parent.height) * (4 / 9)

                onBtnClick: {
                }

                onBtnDelClick: Code.doDel()
            }//Keypad

            ActionButtons {
                color: window.color

                width: (!landscape?parent.width:parent.height)
                height: (landscape?parent.width:parent.height) * (1 / 9)

                onSigCall: window.sigCall(wDisp.txtEd.text)
                onSigText: window.sigText(wDisp.txtEd.text)
                onSigContacts: window.sigContacts()
                onSigInbox: window.sigInbox()

                onSigDel: Code.doDel()
            }
        }//Column
    }//Rectangle
}//Rectangle
