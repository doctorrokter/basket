import bb.cascades 1.4
import bb.device 1.4
import bb.system 1.2
import "../components"

Page {
    id: root
    
    property string path: "/"
    property string name: ""
    property string cursor: ""
    property bool hasMore: true
    property int limit: 50
    property variant account: undefined
    property variant spaceUsage: undefined
    
    property int bytesInGB: 1073741824
    
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    actionBarVisibility: ChromeVisibility.Overlay
    
    titleBar: defaultTitleBar
    
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        layout: DockLayout {}
        
        Container {
            verticalAlignment: VerticalAlignment.Center
            
            Header {
                id: header
                visible: path !== ""
                title: path
            }
            
//            FilesMover {
//                path: root.path
//            }
//            
//            FilesUploader {
//                path: root.path
//                
//                onStartUpload: {
//                    uploadsPageRequested();
//                }
//            }

            ListView {
                id: listView
                scrollRole: ScrollRole.Main
                
                property string currentPath: root.path
                
                verticalAlignment: VerticalAlignment.Bottom
                
                margin.leftOffset: ui.du(0.5)
                margin.rightOffset: ui.du(0.5)
                
                dataModel: ArrayDataModel {
                    id: dataModel
                }
                
                layout: {
                    var view = _app.prop("files_view", "stack");
                    if (view === "grid") {
                        return gridListLayout;
                    }
                    return stackListLayout;
                }
                
                attachedObjects: [
                    ListScrollStateHandler {
                        onScrollingChanged: {
                           if (atEnd && !spinner.running && root.hasMore) {
                              spinner.start();
                              _qdropbox.listFolderContinue(root.cursor);
                           }
                        }
                    }
                ]
                
                function itemType(data, indexPath) {
                    if (layout.objectName === "stackListLayout") {
                        return "listItem";
                    } else {
                        return "gridItem";
                    }
                }
                
                listItemComponents: [
                    ListItemComponent {
                        type: "listItem"
                        StackListItem {
                            tag: ListItemData[".tag"]
                            name: ListItemData.name
                            pathLower: ListItemData.path_lower
                            pathDisplay: ListItemData.path_display
                            fileId: ListItemData.id
                            sharedFolderId: ListItemData.shared_folder_id
                            sharingInfo: ListItemData.sharing_info
                            size: ListItemData.size || 0
                            rev: ListItemData.rev || ""
                            contentHash: ListItemData.content_hash || ""
                            clientModified: ListItemData.client_modified || ""
                            serverModified: ListItemData.server_modified || ""
                            mediaInfo: ListItemData.media_info
                            currentPath: ListItem.view.currentPath
                        }
                    },
                    
                    ListItemComponent {
                        type: "gridItem"
                        GridListItem {
                            tag: ListItemData[".tag"]
                            name: ListItemData.name
                            pathLower: ListItemData.path_lower
                            pathDisplay: ListItemData.path_display
                            fileId: ListItemData.id
                            sharedFolderId: ListItemData.shared_folder_id
                            sharingInfo: ListItemData.sharing_info
                            size: ListItemData.size || 0
                            rev: ListItemData.rev || ""
                            contentHash: ListItemData.content_hash || ""
                            clientModified: ListItemData.client_modified || ""
                            serverModified: ListItemData.server_modified || ""
                            mediaInfo: ListItemData.media_info
                            currentPath: ListItem.view.currentPath
                        }
                    }
                ]
            }
        }
        
        ActivityIndicator {
            id: spinner
            minWidth: ui.du(20)
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }
    }
    
    actions: [
        ActionItem {
            id: createFolder
            title: qsTr("Create folder") + Retranslate.onLocaleOrLanguageChanged
            imageSource: "asset:///images/ic_add_folder.png"
            ActionBar.placement: ActionBarPlacement.Signature
            
            shortcuts: SystemShortcut {
                type: SystemShortcuts.CreateNew
                
                onTriggered: {
                    createFolder.triggered();
                }
            }
            
            onTriggered: {
                root.titleBar = inputTitleBar;
                root.titleBar.focus();
            }
        }
    ]
    
    attachedObjects: [
        GridListLayout {
            id: gridListLayout
            objectName: "gridListLayout"
            columnCount: {
                if (rootDisplayInfo.pixelSize.width === 1440) {
                    return 4;
                }
                return 3;
            }
        },
        
        StackListLayout {
            id: stackListLayout
            objectName: "stackListLayout"
        },
        
        DisplayInfo {
            id: rootDisplayInfo
        },
        
        InputTitleBar {
            id: inputTitleBar    
            
            onCancel: {
                inputTitleBar.reset();
                root.titleBar = defaultTitleBar;
            }
            
            onSubmit: {
                if (text !== "") {
                    spinner.start();
                    var fullPath = (root.path === "" ? "/" : root.path) + text;
                    _qdropbox.createFolder(fullPath);
                    root.titleBar.reset();
                    root.titleBar = defaultTitleBar;
                }
            }
        },
        
        TitleBar {
            id: defaultTitleBar
            
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                
                content: Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    
                    leftPadding: ui.du(1)
                    topPadding: ui.du(0.5)
                    rightPadding: ui.du(1)
                    bottomPadding: ui.du(1)
                    
                    layout: DockLayout {}
                    
                    Label {
                        visible: root.path !== "/"
                        verticalAlignment: VerticalAlignment.Center
                        text: root.name
                        textStyle.base: SystemDefaults.TextStyles.TitleText
                        textStyle.fontWeight: FontWeight.W500
                    }
                    
                    Label {
                        id: fioLabel
                        visible: root.account !== undefined
                        verticalAlignment: VerticalAlignment.Top
                        textStyle.fontWeight: FontWeight.W500
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 10
                        text: root.account === undefined ? "" : root.account.name.display_name
                    }
                    
                    Label {
                        id: loginLabel
                        visible: root.account !== undefined
                        verticalAlignment: VerticalAlignment.Bottom
                        textStyle.base: SystemDefaults.TextStyles.SubtitleText
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 7
                        text: root.account === undefined ? "" : root.account.email
                    }
                    
                    Label {
                        id: bytesLabel
                        visible: root.spaceUsage !== undefined
                        verticalAlignment: VerticalAlignment.Bottom
                        horizontalAlignment: HorizontalAlignment.Right
                        textStyle.base: SystemDefaults.TextStyles.SubtitleText
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 7
                    }
                }
            }
        }
    ]
    
    onSpaceUsageChanged: {
        if (spaceUsage !== undefined) {
            bytesLabel.text = (Number(spaceUsage.used / root.bytesInGB).toFixed(1) + qsTr("GB") + Retranslate.onLocaleOrLanguageChanged) + 
            "/" + (Number(spaceUsage.allocation.allocated / root.bytesInGB).toFixed(1) + qsTr("GB") + Retranslate.onLocaleOrLanguageChanged);
        }
    }
    
    function listFolderLoaded(path, files, cursor, hasMore) {
        spinner.stop();
        if (root.path === path) {
            root.cursor = cursor;
            root.hasMore = hasMore;
            dataModel.append(files);
        }
    }
    
    function listFolderContinueLoaded(files, prevCursor, cursor, hasMore) {
        spinner.stop();
        if (root.cursor === prevCursor) {
            root.cursor = cursor;
            root.hasMore = hasMore;
            dataModel.append(files);
        }
    }
    
    function folderCreated(folder) {
        spinner.stop();
        var p = folder.path_lower;
        if (root.isInRoot(p) || p.indexOf(root.path) !== -1) {
            dataModel.insert(0, folder);
            listView.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.Smooth);
        }
    }
    
    function fileDeleted(file) {
        var p = file.path_lower;
        if (root.isInRoot(p) || p.indexOf(root.path) !== -1) {
            for (var i = 0; i < dataModel.size(); i++) {
                if (dataModel.value(i).id === file.id) {
                    dataModel.removeAt(i);
                }
            }
        }
    }
    
    function moved(file) {
        var p = file.path_lower;
        if (root.isInRoot(p) || p.indexOf(root.path) !== -1) {
            for (var i = 0; i < dataModel.size(); i++) {
                if (dataModel.value(i).id === file.id) {
                    dataModel.replace(i, file);
                }
            }
        }
    }
    
    function isInRoot(path) {
        return root.path === "" && path.split("/").length === 2;
    }
    
    function cleanUp() {
        _qdropbox.popPath();
        _qdropbox.listFolderLoaded.disconnect(root.listFolderLoaded);
        _qdropbox.listFolderContinueLoaded.disconnect(root.listFolderContinueLoaded);
        _qdropbox.folderCreated.disconnect(root.folderCreated);
        _qdropbox.fileDeleted.disconnect(root.fileDeleted);
        _qdropbox.moved.disconnect(root.moved);
    }
    
    onCreationCompleted: {
        _qdropbox.listFolderLoaded.connect(root.listFolderLoaded);
        _qdropbox.listFolderContinueLoaded.connect(root.listFolderContinueLoaded);
        _qdropbox.folderCreated.connect(root.folderCreated);
        _qdropbox.fileDeleted.connect(root.fileDeleted);
        _qdropbox.moved.connect(root.moved);
    }
    
    onPathChanged: {
        spinner.start();
        _qdropbox.listFolder(root.path, root.limit);
    }
}
