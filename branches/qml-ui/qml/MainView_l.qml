import Qt 4.7

Rectangle {
    id: wMainView
    color: "black"
    width: 250; height: 400

    property variant landscape: wMainView.width > wMainView.height
    property variant rotationDelta: landscape? -90 : 0

    Flow {
        anchors.fill: parent
        spacing: 2

        DialDisp {
            id: wDisp
            color: wMainView.color
            rotation: rotationDelta

            width: parent.width
            height: 180
        }//DialDisp

        Keypad {
            color: wMainView.color
            rotation: rotationDelta
            onBtnClick: {
                var origStart = wDisp.tEd.selectionStart;
                var result = wDisp.tEd.text.substr(0,origStart);
                result += strText;
                result += wDisp.tEd.text.substr(wDisp.tEd.selectionEnd);
                wDisp.tEd.text = result;
                wDisp.tEd.cursorPosition = origStart + strText.length;
            }

            width: parent.width
            height: 180
        }//Keypad

        CallText {
            width: parent.width
            height: 40
            rotation: rotationDelta
        }

        ContactsInbox {
            width: parent.width
            height: 40
            rotation: rotationDelta
        }
    }//Flow
}//Rectangle
