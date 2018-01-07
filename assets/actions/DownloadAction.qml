import bb.cascades 1.4

ActionItem {
    id: downloadAction
    
    property ListView listView: undefined
    property bool isDir: false
    
    title: qsTr("Download") + Retranslate.onLocaleOrLanguageChanged
    imageSource: "asset:///images/ic_download.png"
    
    onTriggered: {
        var file = listView.dataModel.data(listView.selected());
        if (downloadAction.isDir) {
            _qdropbox.downloadZip(file.path_display);
        } else {
            _qdropbox.download(file.path_display);
        }
        _app.toast((qsTr("Download started: ") + Retranslate.onLocaleOrLanguageChanged) + _file.filename(file.path_display));
    }
}