import Qt 4.7

Rectangle {
    id: listOfPhones
    color: "black"
    property alias model: listView.model
    signal selectionChanged(int iIndex)

    Row {
        id: row

        width: parent.width
        height: 20

        spacing: 0

        Text {
            id: txtTitle
            width: (parent.width - exitRect.width - 1)
            height: parent.height
            text: "Phone list"
            color: "white"
            font.pointSize: 12; font.bold: true
        }

        Rectangle {
            id: exitRect
            width: 20
            height: parent.height
            border.color: "grey"
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
        id: listView
        width: parent.width
        height: (parent.height - row.height)
        y: row.height

        delegate: Text {
            id: textDelegate
            text: name + " : " + number;
            color: "white"
            font.pointSize: 10

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.debug ("Clicked: " + text);
                    console.debug ("current selection = " + index);
                    listOfPhones.selectionChanged(index);
                    listOfPhones.destroy();
                }
            }
        }

    }//ListView
}//Component: listOfPhones
