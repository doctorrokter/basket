import bb.cascades 1.4
import chachkouski.type 1.0

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
    
    function addToQueue(queue) {
        downloadsDadaModel.append(queue);
    }
    
    function removeFromQueue(filename, path) {
        for (var i = 0; i < downloadsDadaModel.size(); i++) {
            var data = downloadsDadaModel.value(i);
            if (data.path === path) {
                downloadsDadaModel.removeAt(i);
            }
        }
    }
    
    function updateProgress(path, received, total) {
        for (var i = 0; i < downloadsDadaModel.size(); i++) {
            var data = downloadsDadaModel.value(i);
            if (data.path === path) {
                var newData = {
                    filename: data.filename, path: data.path, received: received, total: total
                };
                downloadsDadaModel.replace(i, newData);
            }
        }
    }
    
    function refresh() {
        downloadsDadaModel.clear();
        downloadsDadaModel.append(_qdropbox.downloads);
    }
    
    function cleanUp() {
        _qdropbox.downloadsChanged.disconnect(root.downloadsChanged);
        _qdropbox.downloaded.disconnect(root.downloaded);
        _qdropbox.downloadProgress.disconnect(root.downloadProgress);
    }
    
    function downloadsChanged(downloads) {
        
    }
    
    function downloaded(path, localPath) {
    }
    
    function downloadProgress(path, received, total) {
        
    }
    
    onCreationCompleted: {
        _qdropbox.downloadsChanged.connect(root.downloadsChanged);
        _qdropbox.downloaded.connect(root.downloaded);
        _qdropbox.downloadProgress.connect(root.downloadProgress);
        refresh();
    }
}
