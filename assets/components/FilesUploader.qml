import bb.cascades 1.4

Container {
    id: root
    
    property string path: ""
    
    signal startUpload();
    
    horizontalAlignment: HorizontalAlignment.Fill
    
    layout: DockLayout {}
    
    Button {
        horizontalAlignment: HorizontalAlignment.Left
        text: qsTr("Cancel") + Retranslate.onLocaleOrLanguageChanged
        maxWidth: ui.du(25)
        margin.leftOffset: ui.du(2)
        margin.topOffset: ui.du(2)
        margin.bottomOffset: ui.du(2)
    }
    
    Label {
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
        text: qsTr("Selected") + Retranslate.onLocaleOrLanguageChanged + " " + 0
    }
    
    Button {
        horizontalAlignment: HorizontalAlignment.Right
        text: qsTr("Upload") + Retranslate.onLocaleOrLanguageChanged
        maxWidth: ui.du(25)
        margin.topOffset: ui.du(2)
        margin.rightOffset: ui.du(2)
        margin.bottomOffset: ui.du(2)
        
        onClicked: {
            startUpload();
        }
    }
    
    Divider {
        verticalAlignment: VerticalAlignment.Bottom
    }
}