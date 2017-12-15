import bb.cascades 1.4

ActionItem {
    id: copySharedLinkAction
    
    property ListView listView: undefined
    
    title: qsTr("Copy shared link") + Retranslate.onLocaleOrLanguageChanged
    imageSource: "asset:///images/ic_copy_link.png"
    
    onTriggered: {
        var data = listView.dataModel.data(listView.selected());
        _app.copyToClipboard(data.url);
    }
}
