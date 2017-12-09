import bb.cascades 1.4

ActionItem {
    id: propsAction
    
    property ListView listView: undefined
    
    title: qsTr("Properties") + Retranslate.onLocaleOrLanguageChanged
    imageSource: "asset:///images/ic_properties.png"
    
    onTriggered: {
        var data = listView.dataModel.data(listView.selected());
        listView.requestPropsPage(data);
    }
    
    shortcuts: [
        Shortcut {
            key: "p"
            
            onTriggered: {
                propsAction.triggered();
            }
        }
    ]
}
