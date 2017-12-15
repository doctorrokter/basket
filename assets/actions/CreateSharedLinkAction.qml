import bb.cascades 1.4

ActionItem {
    id: createSharedLinkAction
    
    property ListView listView: undefined
    
    title: qsTr("Create shared link") + Retranslate.onLocaleOrLanguageChanged
    imageSource: "asset:///images/ic_link.png"
    
    onTriggered: {
        var data = listView.dataModel.data(listView.selected());
        _qdropbox.createSharedLink(data.path_display);
    }
    
    shortcuts: Shortcut {
        key: "l"
        
        onTriggered: {
            createSharedLinkAction.triggered();
        }
    }
}
