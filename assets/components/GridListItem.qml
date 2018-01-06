import bb.cascades 1.4
import bb.device 1.4
import bb.system 1.2
import components 1.0
import "../actions"

CustomListItem {
    id: root
    
    property string tag: ""
    property string name: ""
    property string pathLower: ""
    property string pathDisplay: ""
    property string fileId: ""
    property string sharedFolderId: ""
    property variant sharingInfo: undefined
    property int size: 0
    property string rev: ""
    property string contentHash: ""
    property string clientModified: ""
    property string serverModified: ""
    property variant mediaInfo: undefined
    property variant membersCount: 0
    property string membersCursor: ""
    property bool isOwner: false
    property string url: ""
    
    property string thumbnail: ""
    property string currentPath: ""
    
    opacity: root.ListItem.selected ? 0.5 : 1.0
    
    function getTextStyle() {
        if (displayInfo.pixelSize.width === 1440) {
            return SystemDefaults.TextStyles.BodyText;
        }
        return SystemDefaults.TextStyles.SubtitleText;
    }
    
    function isDir() {
        return root.tag === "folder";
    }
    
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        background: ui.palette.plain
        
        layout: DockLayout {}
        
        ImageView {
            id: preview
            
            opacity: root.isDir() ? 0.25 : 1.0
            preferredWidth: root.isDir()  ? listItemLUH.layoutFrame.width : ui.du(20)
            preferredHeight: {
                if (root.dir) {
                    return listItemLUH.layoutFrame.height - ui.du(2);
                } else {
                    return ui.du(20);
                } 
            }
            verticalAlignment: {
                if (root.isDir()) {
                    return VerticalAlignment.Bottom;
                } else {
                    return VerticalAlignment.Center;
                }
            }
            horizontalAlignment: root.isDir() ? HorizontalAlignment.Fill : HorizontalAlignment.Center
        }
        
        Container {
            id: mainImageContainer
        }
        
        ImageView {
            id: bgImage
            visible: !root.isDir()
            imageSource: "asset:///images/opac_bg.png"
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Bottom
            filterColor: Color.Black
            opacity: 0.8
            maxHeight: listItemLUH.layoutFrame.height / 2
        }
        
        Label {
            id: fileName
            verticalAlignment: VerticalAlignment.Bottom
            textStyle.base: SystemDefaults.TextStyles.BodyText
            
            margin.leftOffset: ui.du(1);
            margin.bottomOffset: root.isDir() ? ui.du(5.5) : ui.du(4)
            
            textStyle.color: root.isDir() ? ui.palette.textOnPlain : ui.palette.textOnPrimary
        }
        
        Label {
            visible: !root.isDir()
            
            text: _date.str(root.clientModified);
            verticalAlignment: VerticalAlignment.Bottom
            textStyle.color: ui.palette.textOnPrimary
            textStyle.base: SystemDefaults.TextStyles.SmallText
            textStyle.fontWeight: FontWeight.W100
            
            margin.leftOffset: ui.du(1);
            margin.bottomOffset: ui.du(1)
        }
        
        Container {
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Top
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            
            ImageView {
                visible: root.url !== ""
                verticalAlignment: VerticalAlignment.Center
                imageSource: "asset:///images/ic_link.png"
                maxWidth: ui.du(5)
                maxHeight: ui.du(5)
                filterColor: ui.palette.secondaryTextOnPlain
            }
            
            Container {
                verticalAlignment: VerticalAlignment.Center
                margin.leftOffset: ui.du(2)
                visible: root.membersCount > 0
                
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                Container {
                    
                    verticalAlignment: VerticalAlignment.Center
                    Label {
                        text: root.membersCount
                        textStyle.color: ui.palette.secondaryTextOnPlain
                        textStyle.base: SystemDefaults.TextStyles.BodyText
                    }
                }
                
                ImageView {
                    verticalAlignment: VerticalAlignment.Center
                    id: sharedFolder
                    imageSource: "asset:///images/ic_groups_white.png"
                    filterColor: ui.palette.secondaryTextOnPlain
                    maxWidth: ui.du(4.8)
                    maxHeight: ui.du(4.8)
                }
            }
        }
        
        
        attachedObjects: [
            LayoutUpdateHandler {
                id: listItemLUH
            }
        ]
    }
    
    attachedObjects: [
        DisplayInfo {
            id: displayInfo
        },
        
        ComponentDefinition {
            id: dirView
            ImageView {
                imageSource: "asset:///images/ic_folder.png"
                filterColor: ui.palette.primary
                scalingMethod: ScalingMethod.AspectFill
                opacity: 0.25
                preferredWidth: listItemLUH.layoutFrame.width
                preferredHeight: listItemLUH.layoutFrame.height
            }
        },
        
        ComponentDefinition {
            id: fileView
            FileImageView {
                path: root.pathDisplay
                preferredWidth: listItemLUH.layoutFrame.width
                preferredHeight: listItemLUH.layoutFrame.height
            }
        },
        
        ComponentDefinition {
            id: thumbnailView
            ThumbnailImageView {
                size: "w640h480"
                scalingMethod: ScalingMethod.AspectFill
                
                preferredWidth: listItemLUH.layoutFrame.width
                preferredHeight: listItemLUH.layoutFrame.height
            }
        }
    ]
    
    contextActions: [
        ActionSet {
            actions: [
                DeleteFileAction {
                    id: deleteFileAction
                    pathDisplay: root.pathDisplay    
                },
                
                RenameFileAction {
                    id: renameFileAction
                    path: root.pathDisplay
                    isDir: root.isDir()
                },
                
                MoveActionItem {
                    listView: root.ListItem.view
                },
                
                PropsAction {
                    listView: root.ListItem.view
                },
                
                ShareAction {
                    enabled: root.isOwner || root.sharedFolderId === ""
                    listView: root.ListItem.view
                },
                
                CreateSharedLinkAction {
                    listView: root.ListItem.view    
                    enabled: root.url === "" 
                },
                
                CopySharedLinkAction {
                    listView: root.ListItem.view  
                    enabled: root.url !== ""
                },
                
                RevokeSharedLinkAction {
                    listView: root.ListItem.view  
                    enabled: root.url !== ""
                },
                
                DownloadAction {
                    listView: root.ListItem.view
                    enabled: !root.isDir()
                }
            ]
        }
    ]
    
    onSharedFolderIdChanged: {
        sharedFolder.visible = sharedFolderId !== "";
    }
    
    onNameChanged: {
        fileName.text = name;
        renameFileAction.name = name;
    }
    
    onFileIdChanged: {
        mainImageContainer.removeAll();
        if (root.isDir()) {
            mainImageContainer.add(dirView.createObject());
        } else {
            mainImageContainer.add(fileView.createObject());
            var ext = _file.extension(root.name).toLowerCase();
            if (_file.isImage(ext)) {
                var thumb = thumbnailView.createObject();
                thumb.loaded.connect(function() {
                    mainImageContainer.removeAll();
                    mainImageContainer.add(thumb);
                });
                thumb.path = root.pathDisplay;
            }
        }
    }
}
