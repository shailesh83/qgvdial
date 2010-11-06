import Qt 4.7
import "helper.js" as Code

Rectangle {
    id: listOfPhones
    color: "black"
    property alias model: listView.model
    property bool alive: false

    signal selectionChanged(int iIndex)
    signal sigHeightChanged(int iHeight)
    signal sigDestructor

    function calcFontPoint () {
        var pt = Math.max(listOfPhones.width,listOfPhones.height);
        pt = pt / 20;
        if (pt < 1) pt = 1;
        sigHeightChanged(pt);
        return pt;
    }

    states: [
        State{ name: "AliveState"; when: alive == true
            PropertyChanges { target: listView; opacity: 1 }
        },
        State{ name: "DeadState"; when: alive == false
            PropertyChanges { target: listView; opacity: 0 }
        }
    ]

    ListView {
        id: listView
        anchors.fill: parent

        // Display smoothly
        Behavior on opacity {
            NumberAnimation { properties:"opacity"; duration: 300 }
        }

        delegate: Rectangle {
            id: rectDelegate
            color: "black"
            border.color: "grey"
            width: listView.width
            height: (txtName.height + txtNumber.height)

            Column {
                Text {
                    id: txtName
                    text: name
                    color: "white"
                    width: listView.width
                    font.pointSize: calcFontPoint ();
                    elide: Text.ElideMiddle
                }

                Text {
                    id: txtNumber
                    text: number;
                    color: "white"
                    width: listView.width
                    font.pointSize: calcFontPoint ();
                    elide: Text.ElideMiddle
                }
            }// Column

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
