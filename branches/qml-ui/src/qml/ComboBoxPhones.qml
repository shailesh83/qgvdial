import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: listOfPhones
    color: "black"
    property alias model: listView.model

    signal selectionChanged(int iIndex)
    signal sigHeightChanged(int iHeight)
    signal sigDestructor

    function calcFontPoint () {
        var pt = (listOfPhones.width + listOfPhones.height) / 2;
        pt = pt / 10;
        if (pt < 1) pt = 1;
        console.debug ("pt = " + pt);
        sigHeightChanged(pt);
        return pt;
    }

    Row {
        id: row

        width: parent.width
        height: (parent.height / 3)

        spacing: 0

        Text {
            id: txtTitle
            width: (parent.width - exitRect.width - 1)
            height: parent.height
            text: "Phone list"
            color: "white"
            font.pointSize: calcFontPoint ();
        }

        Rectangle {
            id: exitRect
            width: (txtX.font.pointSize + 3)
            height: parent.height
            border.color: "grey"
            color: "red"
            smooth: true
            radius: ((height + width) / 20);

            Text {
                id: txtX
                text: "X"

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter

                font.pointSize: calcFontPoint ();
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    listOfPhones.sigDestructor();
                    listOfPhones.destroy();
                }
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
            font.pointSize: calcFontPoint ();

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    listOfPhones.selectionChanged(index);
                    listOfPhones.sigDestructor();
                    listOfPhones.destroy();
                }
            }
        }
    }//ListView
}//Component: listOfPhones
