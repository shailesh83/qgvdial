import Qt 4.7
import org.qgvdial.WebWidget 1.0

Item {
    id: container

    signal sigBack()

    Column {
        id: mainColumn
        anchors.fill: parent
        anchors.topMargin: 2
        spacing: 1

        property int pixDiv: 15
        property int pixHeight: (container.height + container.width) / 2
        property int pixSize: pixHeight / pixDiv

        MyWebWidget {
            height: container.height - btnBack.height - mainColumn.spacing
            width: parent.width
        }

        MyButton {
            id: btnBack

            mainText: "Back"
            width: parent.width
            mainPixelSize: mainColumn.pixSize

            onClicked: container.sigBack();
        }//MyButton (Back)
    }//Column
}//Item (container)
