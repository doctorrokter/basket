import bb.cascades 1.4

Page {
    id: root
    
    titleBar: TitleBar {
        title: qsTr("Uploads") + Retranslate.onLocaleOrLanguageChanged
    }
    
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        ListView {
            id: downloadsListView
            
            dataModel: ArrayDataModel {
                id: downloadsDadaModel
            }
            
            listItemComponents: [
                ListItemComponent {
                    CustomListItem {
                        id: listItem
                        
                        Container {
                            horizontalAlignment: HorizontalAlignment.Fill
                            verticalAlignment: VerticalAlignment.Fill
                            
                            margin.leftOffset: ui.du(1)
                            margin.topOffset: ui.du(1)
                            margin.rightOffset: ui.du(1)
                            margin.bottomOffset: ui.du(1)
                            
                            Label {
                                text: ListItemData.filename
                                textStyle.base: SystemDefaults.TextStyles.SubtitleText
                            }
                            
                            ProgressIndicator {
                                id: progress
                                
                                fromValue: 0
                                toValue: ListItemData.total
                                value: ListItemData.received
                            }
                        }                        
                    }
                }
            ]
        }
    }
    
    function refresh() {
        downloadsDadaModel.clear();
        var uploads = _qdropbox.uploads.map(function(path) {
            return createDownload(path);    
        });
        downloadsDadaModel.append(uploads);
    }
    
    function cleanUp() {
        _qdropbox.uploadStarted.disconnect(root.uploadStarted);
        _qdropbox.uploaded.disconnect(root.uploaded);
        _qdropbox.uploadProgress.disconnect(root.uploadProgress);
    }
    
    function uploadStarted(path) {
        downloadsDadaModel.append(createDownload(path));      
    }
    
    function uploaded(file) {
        for (var i = 0; i < downloadsDadaModel.size(); i++) {
            if (downloadsDadaModel.value(i).path === file.path_display) {
                downloadsDadaModel.removeAt(i);
                return;
            }
        }
    }
    
    function uploadProgress(path, received, total) {
        for (var i = 0; i < downloadsDadaModel.size(); i++) {
            var d = downloadsDadaModel.value(i);
            if (d.path === path) {
                d.received = received;
                if (d.total === 0) {
                    d.total = total;
                }
                downloadsDadaModel.replace(i, d);
                return;
            }
        }
    }
    
    function createDownload(path) {
        return {
            filename: _file.filename(path),
            path: path, 
            received: 0, 
            total: 0
        };
    }
    
    onCreationCompleted: {
        if (_startupMode === "Card") {
            root.parent.peekEnabled = false;
            root.actionBarVisibility = ChromeVisibility.Hidden;
        }
        
        _qdropbox.uploadStarted.connect(root.uploadStarted);
        _qdropbox.uploaded.connect(root.uploaded);
        _qdropbox.uploadProgress.connect(root.uploadProgress);
        refresh();
    }
}
