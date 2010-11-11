import Qt 4.7

Rectangle {
    id: wMainView
    color: "black"
    width: 250; height: 400

    signal sigCall(string strNumber)
    signal sigText(string strNumber)

    property variant landscape: wMainView.width > wMainView.height
    property variant rotationDelta: landscape? -90 : 0

    function doDel () {
        var origStart = wDisp.txtEd.selectionStart;
        var sel = wDisp.txtEd.selectionEnd - origStart;
        var result = wDisp.txtEd.text.substr(0,origStart);
        if (sel == 0) {
            result = result.substr(0,origStart-1);
        }
        result += wDisp.txtEd.text.substr(wDisp.txtEd.selectionEnd);
        wDisp.txtEd.text = result;

        if (origStart > result.length) {
            origStart = result.length;
        }

        wDisp.txtEd.cursorPosition = origStart;
    }

    Flow {
        anchors.fill: parent
        spacing: 2

        DialDisp {
            id: wDisp
            color: wMainView.color
            rotation: rotationDelta

            width: parent.width
            height: wMainView.height * (4 / 9)
        }//DialDisp

        Keypad {
            color: wMainView.color
            rotation: rotationDelta

            width: parent.width
            height: wMainView.height * (4 / 9)

            onBtnClick: {
                var origStart = wDisp.txtEd.selectionStart;
                var result = wDisp.txtEd.text.substr(0,origStart);
                result += strText;
                result += wDisp.txtEd.text.substr(wDisp.txtEd.selectionEnd);
                wDisp.txtEd.text = result;
                wDisp.txtEd.cursorPosition = origStart + strText.length;
            }

            onBtnDelClick: doDel()
        }//Keypad

        ActionButtons {
            id: btnSpecial
            width: parent.width
            height: wMainView.height * (1 / 9)
            rotation: rotationDelta

            onSigCall: wMainView.sigCall(wDisp.txtEd.text)
            onSigText: wMainView.sigText(wDisp.txtEd.text)

            onSigDel: doDel()
        }
    }//Flow
}//Rectangle
