import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: wDisp

    // Expose the text edit as a property
    property TextEdit tEd: txtNum

    width: Code.calcFlowChildWidth();
    height: Code.calcFlowChildHeight();

    Column {
        anchors.fill: parent

        Rectangle {
            color: "black"
            width: parent.width
            height: (parent.height / 5)

            MyButton {
                id: btnPhones
                mainText: "a"
                anchors.fill: parent
                radius: ((height / 10.0) + (width / 60.0))

                onClicked: {
                    var comp = Qt.createComponent ("ComboBoxPhones.qml");
                    var sprite = comp.createObject (wDisp.parent);
                    sprite.width = width;
                    sprite.height = 100;
                    sprite.y = y + height;
                }
            }
        }

        TextEdit {
            id: txtNum
            width: parent.width

            color: "white"
            textFormat: TextEdit.PlainText
            cursorVisible: true
            wrapMode: TextEdit.WrapAnywhere
            selectByMouse: true
            font {
                pointSize: (Code.btnFontPoint()/3);
                bold: true
            }

            height: (parent * 4 / 5)
        }// TextEdit
    }// Flow
}// Rectangle
