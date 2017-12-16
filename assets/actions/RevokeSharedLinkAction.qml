import bb.cascades 1.4

ActionItem {
    id: revokeAction
    
    property ListView listView: undefined
    
    title: qsTr("Revoke shared link") + Retranslate.onLocaleOrLanguageChanged
    imageSource: "asset:///images/ic_clear.png"
    
    onTriggered: {
        var file = listView.dataModel.data(listView.selected());
        _qdropbox.revokeSharedLink(file.url);
    }
}
