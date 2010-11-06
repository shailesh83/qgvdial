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
        var pt = Math.max(listOfPhones.width,listOfPhones.height);
        pt = pt / 20;
        if (pt < 1) pt = 1;
        sigHeightChanged(pt);
        return pt;
    }

    ListView {
        id: listView
        anchors.fill: parent

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
