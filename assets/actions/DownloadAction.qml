import bb.cascades 1.4

ActionItem {
    id: downloadAction
    
    property ListView listView: undefined
    
    title: qsTr("Download") + Retranslate.onLocaleOrLanguageChanged
    imageSource: "asset:///images/ic_download.png"
    
    onTriggered: {
        var file = listView.dataModel.data(listView.selected());
        _qdropbox.download(file.path_display);
    }
}