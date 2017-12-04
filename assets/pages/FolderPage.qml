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
    
    titleBar: TitleBar {
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
//                
//                Label {
//                    id: bytesLabel
//                    visible: path === "/" && _userController.user !== null
//                    verticalAlignment: VerticalAlignment.Bottom
//                    horizontalAlignment: HorizontalAlignment.Right
//                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
//                    textStyle.fontSize: FontSize.PointValue
//                    textStyle.fontSizeValue: 7
//                }
            }
        }
    }
    
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
                    var view = _app.prop("files_view", "grid");
                    if (view === "grid") {
                        return gridListLayout;
                    }
                    return stackListLayout;
                }
                
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
    }
    
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
        }
    ]
    
    function listFolderLoaded(path, files, cursor, hasMore) {
        if (root.path === path) {
            root.cursor = cursor;
            root.hasMore = hasMore;
            dataModel.append(files);
        }
    }
    
    function cleanUp() {
        _qdropbox.popPath();
        _qdropbox.listFolderLoaded.disconnect(root.listFolderLoaded);
    }
    
    onCreationCompleted: {
        _qdropbox.listFolderLoaded.connect(root.listFolderLoaded);
    }
    
    onPathChanged: {
        _qdropbox.listFolder(root.path, root.limit);
    }
}
