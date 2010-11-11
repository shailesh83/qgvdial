////////////////////////////////////////////////////////////////////////////////
// All helper functions and their global variables sit in this file
////////////////////////////////////////////////////////////////////////////////
var compCbBox, cbBox;

////////////////////////////////////////////////////////////////////////////////
// Calculate the height/width of the current component
function calcFlowChildHeight () {
    if (parent.height > parent.width) {
        return (parent.height / 2);
    } else {
        return (parent.height);
    }
}
function calcFlowChildWidth () {
    if (parent.width > parent.height) {
        return ((parent.width - parent.spacing) / 2);
    } else {
        return (parent.width);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Calculate the font point from the minimum of the height and width of the button
function btnFontPoint () {
    var m = (height < width? height : width);
    m = (m + 1) / 2;
    m = (m < 1 ? 1 : m);
    return (m);
}

////////////////////////////////////////////////////////////////////////////////
// Calculate the subtext font point from the main font point
function btnSubTextFontPoint () {
    var m = (btnFontPoint() + 2) / 3;
    m = (m == 0 ? 1 : m);
    return (m);
}

// Deletion in the MainView
function doDel () {
    var origStart = wDisp.txtEd.selectionStart;
    var sel = wDisp.txtEd.selectionEnd - origStart;
    var result = wDisp.txtEd.text.substr(0,origStart);
    if (sel == 0) {
        result = result.substr(0,origStart-1);
    }
    result += wDisp.txtEd.text.substr(wDisp.txtEd.selectionEnd);
    wDisp.txtEd.text = result;

    if (origStart > result.length) {
        origStart = result.length;
    }

    wDisp.txtEd.cursorPosition = origStart;
}


function getAngle(orientation) {
    var angle;
    if (orientation == Orientation.Portrait) {
        angle = 0;
    } else if (orientation == Orientation.Landscape) {
        angle = 90;
    } else if (orientation == Orientation.PortraitInverted) {
        angle = 180;
    } else if (orientation == Orientation.LandscapeInverted) {
        angle = 270;
    } else {
        angle = 0;
    }
    return angle;
}
