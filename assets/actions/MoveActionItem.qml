import bb.cascades 1.4

ActionItem {
    id: moveAction
    
    property ListView listView: undefined
    
    title: qsTr("Move") + Retranslate.onLocaleOrLanguageChanged
    imageSource: "asset:///images/ic_forward.png"
    
    onTriggered: {
        var indexPath = listView.selected();
        _qdropbox.select(listView.dataModel.data(indexPath));
    }
    
    shortcuts: [
        Shortcut {
            key: "m"
            
            onTriggered: {
                moveAction.triggered();
            }
        }
    ]
}
