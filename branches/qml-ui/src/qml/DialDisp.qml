import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: wDisp

    // Expose the text edit as a property
    property TextEdit tEd: txtNum

    width: Code.calcFlowChildWidth();
    height: Code.calcFlowChildHeight();

    ListModel {
        id: myModel
        ListElement {
            name: "all";
            number: "+1 aaa bbb cccc"
        }//ListElement
        ListElement {
            name: "b";
            number: "+1 AAA BBB CCCC"
        }//ListElement
    }//ListModel


    function randomSlot(iIndex) {
        btnPhones.mainText = myModel.get(iIndex).name;
    }

    Column {
        anchors.fill: parent

        Rectangle {
            color: "black"
            width: parent.width
            height: (parent.height / 5)

            MyButton {
                id: btnPhones
                mainText: myModel.get(0).name;
                anchors.fill: parent
                radius: ((height / 10.0) + (width / 60.0))

                onClicked: {
                    var comp = Qt.createComponent ("ComboBoxPhones.qml");
                    var cbBox = comp.createObject (wDisp.parent);
                    cbBox.model = myModel;
                    cbBox.width = width;
                    cbBox.height = cbBox.model.count * 30;
                    cbBox.y = y + height;
                    cbBox.selectionChanged.connect(randomSlot);
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
