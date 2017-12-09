import bb.cascades 1.4

Page {
    id: root
    
    titleBar: TitleBar {
        title: qsTr("Downloads") + Retranslate.onLocaleOrLanguageChanged
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
        var downloads = _qdropbox.downloads.map(function(path) {
            return createDownload(path);    
        });
        downloadsDadaModel.append(downloads);
    }
    
    function cleanUp() {
        _qdropbox.downloadStarted.disconnect(root.downloadStarted);
        _qdropbox.downloaded.disconnect(root.downloaded);
        _qdropbox.downloadProgress.disconnect(root.downloadProgress);
    }
    
    function downloadStarted(path) {
        downloadsDadaModel.append(createDownload(path));      
    }
    
    function downloaded(path, localPath) {
        for (var i = 0; i < downloadsDadaModel.size(); i++) {
            if (downloadsDadaModel.value(i).path === path) {
                downloadsDadaModel.removeAt(i);
                return;
            }
        }
    }
    
    function downloadProgress(path, received, total) {
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
        _qdropbox.downloadStarted.connect(root.downloadStarted);
        _qdropbox.downloaded.connect(root.downloaded);
        _qdropbox.downloadProgress.connect(root.downloadProgress);
        refresh();
    }
}
