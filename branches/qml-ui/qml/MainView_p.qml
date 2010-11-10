import Qt 4.7

Rectangle {
    id: wMainView
    color: "black"
    width: 250; height: 400

    property variant landscape: wMainView.width > wMainView.height
    property variant rotationDelta: landscape? -90 : 0

    function doDel () {
        var origStart = wDisp.tEd.selectionStart;
        var sel = wDisp.tEd.selectionEnd - origStart;
        var result = wDisp.tEd.text.substr(0,origStart);
        if (sel == 0) {
            result = result.substr(0,origStart-1);
        }
        result += wDisp.tEd.text.substr(wDisp.tEd.selectionEnd);
        wDisp.tEd.text = result;

        if (origStart > result.length) {
            origStart = result.length;
        }

        wDisp.tEd.cursorPosition = origStart;
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
                var origStart = wDisp.tEd.selectionStart;
                var result = wDisp.tEd.text.substr(0,origStart);
                result += strText;
                result += wDisp.tEd.text.substr(wDisp.tEd.selectionEnd);
                wDisp.tEd.text = result;
                wDisp.tEd.cursorPosition = origStart + strText.length;
            }

            onBtnDelClick: doDel()
        }//Keypad

        ActionButtons {
            id: btnSpecial
            width: parent.width
            height: wMainView.height * (1 / 9)
            rotation: rotationDelta

            onSigDel: doDel()
        }
    }//Flow
}//Rectangle
