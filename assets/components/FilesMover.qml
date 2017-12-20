import bb.cascades 1.4

Container {
    id: root
    
    property string path: ""
    
    visible: _qdropbox.selected.length > 0    
    horizontalAlignment: HorizontalAlignment.Fill
    
    layout: DockLayout {}
    
    Button {
        horizontalAlignment: HorizontalAlignment.Left
        text: qsTr("Clear") + Retranslate.onLocaleOrLanguageChanged
        maxWidth: ui.du(25)
        margin.leftOffset: ui.du(2)
        margin.topOffset: ui.du(2)
        margin.bottomOffset: ui.du(2)
        
        onClicked: {
            _qdropbox.unselectAll();
        }
    }
    
    Label {
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
        text: qsTr("Selected") + Retranslate.onLocaleOrLanguageChanged + " " + _qdropbox.selected.length
    }
    
    Button {
        horizontalAlignment: HorizontalAlignment.Right
        text: qsTr("Move") + Retranslate.onLocaleOrLanguageChanged
        maxWidth: ui.du(25)
        margin.topOffset: ui.du(2)
        margin.rightOffset: ui.du(2)
        margin.bottomOffset: ui.du(2)
        
        onClicked: {
            if (_qdropbox.selected.length > 1) {
                _qdropbox.moveBatch(root.path);
            } else {
                _qdropbox.selected.forEach(function(file) {
                    _qdropbox.move(file.path_display, root.path + "/" + file.name);
                });
            }
            
        }
    }
    
    Divider {
        verticalAlignment: VerticalAlignment.Bottom
    }
}