import bb.cascades 1.4

ActionItem {
    id: shareAction
    
    property ListView listView: undefined
    
    title: qsTr("Share") + Retranslate.onLocaleOrLanguageChanged
    imageSource: "asset:///images/ic_share.png"
    
    onTriggered: {
        var data = listView.dataModel.data(listView.selected());
        listView.shareFolder(data.path_display);
    }
    
    shortcuts: [
        Shortcut {
            key: "s"
            
            onTriggered: {
                publishAction.triggered();
            }
        }
    ]
}