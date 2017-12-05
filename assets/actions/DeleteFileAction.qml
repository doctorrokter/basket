import bb.cascades 1.4

DeleteActionItem {
    id: deleteAction
    
    property string pathDisplay: ""
    
    onTriggered: {
        _qdropbox.deleteFile(deleteAction.pathDisplay);
    }
    
    shortcuts: [
        Shortcut {
            key: "d"
            
            onTriggered: {
                deleteAction.triggered();
            }
        }
    ]
}
