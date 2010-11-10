import Qt 4.7

Row {
    width: parent.width
    height: 40
    rotation: rotationDelta

    MyButton {
        id: btnContacts
        mainText: "Contacts"

        width: parent.width / 2
        height: parent.height
    }
    MyButton {
        id: btnInbox
        mainText: "Inbox"

        width: parent.width / 2
        height: parent.height
    }
}
