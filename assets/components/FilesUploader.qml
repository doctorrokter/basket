import bb.cascades 1.4

Container {
    id: root
    
    property string path: ""
    
    signal startUpload();
    
    visible: _app.sharedFiles.length > 0 || _app.sharedUrls.length > 0
    horizontalAlignment: HorizontalAlignment.Fill
    
    layout: DockLayout {}
    
    Button {
        horizontalAlignment: HorizontalAlignment.Left
        text: qsTr("Cancel") + Retranslate.onLocaleOrLanguageChanged
        maxWidth: ui.du(25)
        margin.leftOffset: ui.du(2)
        margin.topOffset: ui.du(2)
        margin.bottomOffset: ui.du(2)
        
        onClicked: {
            _app.sharedFiles = [];
            _app.sharedUrls = [];
            _app.onCardDone("Card done!");
        }
    }
    
    Label {
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
        text: qsTr("Selected") + Retranslate.onLocaleOrLanguageChanged + " " + root.getCount()
    }
    
    Button {
        horizontalAlignment: HorizontalAlignment.Right
        text: qsTr("Upload") + Retranslate.onLocaleOrLanguageChanged
        maxWidth: ui.du(25)
        margin.topOffset: ui.du(2)
        margin.rightOffset: ui.du(2)
        margin.bottomOffset: ui.du(2)
        
        onClicked: {
            if (hasSharedFiles()) {
                _app.shareFiles(root.path);
                _app.sharedFiles = [];
                _app.onCardDone("Card done!");
            }
            
            if (hasSharedUrls()) {
                _app.shareUrls(root.path);
                _app.sharedUrls = [];
                _app.onCardDone("Card done!");
            }
        }
    }
    
    Divider {
        verticalAlignment: VerticalAlignment.Bottom
    }
    
    function hasSharedFiles() {
        return _app.sharedFiles.length > 0;
    }
    
    function hasSharedUrls() {
        return _app.sharedUrls.length > 0;
    }
    
    function getCount() {
        if (hasSharedFiles()) {
            return _app.sharedFiles.length;
        } else if (hasSharedUrls()) {
            return _app.sharedUrls.length;
        } else {
            return 0;
        }
        
    }
}