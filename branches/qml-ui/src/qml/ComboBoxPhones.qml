import Qt 4.7
import "helper.js" as Code

Item {
    id: cbNumbers
    height: (parent.height / 5)

    width: parent.width

    // List of items
//    property list<string> lstItems
    property variant lstItems
    // Currently selected item
    property int currSelected: -1

    MyButton {
        id: btn
        mainText: (currSelected==-1?"...":lstItems[currSelected])
        radius: ((cbNumbers.height / 10.0) + (cbNumbers.width / 60.0))

        onClicked: {
            var comp = Qt.createComponent ("Cb_dropdown.qml");
            var sprite = comp.createObject (cbNumbers.parent.parent.parent);
            sprite.width = btn.width;
//            sprite.height = 100;
            sprite.y = btn.y + btn.height;
        }
    }
}// Item
