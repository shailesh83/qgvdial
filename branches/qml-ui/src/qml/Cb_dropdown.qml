import Qt 4.7

Rectangle {
    id: listOfPhones
    height: 200

    Row {
        id: row
        height: 20; width: parent.width
        spacing: 0

        Text {
            id: txtTitle
            width: (parent.width - exitRect.width - 1)
            height: parent.height
            text: "Phone list"
            font.pointSize: 12; font.bold: true
        }

        Rectangle {
            id: exitRect
            width: 20
            height: parent.height
            border.color: "black"
            color: "red"
            smooth: true
            radius: ((height + width) / 20);

            Text {
                text: "X"

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                font.pointSize: 10
                font.bold: true
            }

            MouseArea {
                anchors.fill: parent
                onClicked: listOfPhones.destroy();
            }
        }
    }

    ListView {
        width: parent.width; height: (parent.height - row.height)
        y: row.height

        model: ListModel {
            id: myModel
            ListElement {
                name: "a";
                number: "+1 aaa bbb cccc"
            }//ListElement
            ListElement {
                name: "b";
                number: "+1 AAA BBB CCCC"
            }//ListElement
        }//ListModel

        delegate: Text {
            id: textDelegate
            text: name + " : " + number;
            font.pointSize: 10

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.debug ("Clicked: " + text);
                }
            }
        }

    }//ListView
}//Component: listOfPhones
