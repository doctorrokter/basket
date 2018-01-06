import bb.cascades 1.4
import bb.system 1.2
import "../actions"
import components 1.0

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
    property int membersCount: 0
    property string membersCursor: ""
    property bool isOwner: false
    property string url: ""
    
    property string thumbnail: ""
    
    property string currentPath: "/"
    
    opacity: root.ListItem.selected ? 0.5 : 1.0
    
    contextActions: [
        ActionSet {
            
            actions: [
                DeleteFileAction {
                    pathDisplay: root.pathDisplay    
                },
                
                RenameFileAction {
                    name: root.name
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
    
    function isDir() {
        return root.tag === "folder";
    }
    
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        layout: StackLayout {
            orientation: LayoutOrientation.LeftToRight
        }
        
        Container {
            background: ui.palette.plain
            
            layout: DockLayout {
                
            }
            
            Container {
                id: mainImageContainer
            }
            
            ImageView {
                id: sharedFolder
                imageSource: "asset:///images/ic_groups_white.png"
                maxWidth: ui.du(5)
                maxHeight: ui.du(5)
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Bottom
                margin.bottomOffset: ui.du(2)
                opacity: 0.5
                filterColor: ui.palette.primary;
                visible: false
            }
        }                            
        
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            
            layoutProperties: StackLayoutProperties {
                spaceQuota: 1
            }
            
            leftPadding: ui.du(1)
            topPadding: {
                if (!root.isDir()) {
                    return ui.du(1);
                }
            }
            rightPadding: ui.du(1)
            bottomPadding: {
                if (!root.isDir()) {
                    return ui.du(1);
                }
            }
            
            verticalAlignment: {
                if (root.isDir()) {
                    return VerticalAlignment.Center
                }
                return VerticalAlignment.Fill
            }
            
            layout: DockLayout {}
            
            Container {
                
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                
                Label {
                    text: root.name
                    textStyle.base: SystemDefaults.TextStyles.BodyText
                    verticalAlignment: VerticalAlignment.Top
                    
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1
                    }
                }
                
                ImageView {
                    visible: root.url !== ""
                    horizontalAlignment: HorizontalAlignment.Right
                    verticalAlignment: VerticalAlignment.Center
                    imageSource: "asset:///images/ic_link.png"
                    maxWidth: ui.du(4.8)
                    maxHeight: ui.du(4.8)
                    filterColor: ui.palette.secondaryTextOnPlain
                }
                
                Container {
                    margin.leftOffset: ui.du(2)
                    verticalAlignment: VerticalAlignment.Center
                    visible: root.membersCount !== 0
                    
                    layout: StackLayout {
                        orientation: LayoutOrientation.LeftToRight
                    }
                    
                    Container {
                        Label {
                            text: root.membersCount
                            textStyle.color: ui.palette.secondaryTextOnPlain
                            textStyle.base: SystemDefaults.TextStyles.BodyText
                        }
                    }
                    
                    ImageView {
                        imageSource: "asset:///images/ic_groups_white.png"
                        filterColor: ui.palette.secondaryTextOnPlain
                        maxWidth: ui.du(4.8)
                        maxHeight: ui.du(4.8)
                        verticalAlignment: VerticalAlignment.Center
                    }
                }
            }
            
            
            Label {
                visible: !root.isDir()
                verticalAlignment: VerticalAlignment.Bottom
                text: _date.str(root.clientModified);
                textStyle.base: SystemDefaults.TextStyles.SubtitleText
                textStyle.fontWeight: FontWeight.W100
            }
            
            Label {
                visible: !root.isDir()
                verticalAlignment: VerticalAlignment.Bottom
                horizontalAlignment: HorizontalAlignment.Right
                text: Number(root.size / (1024 * 1024)).toFixed(1) + " " + qsTr("MB") + Retranslate.onLocaleOrLanguageChanged
                textStyle.base: SystemDefaults.TextStyles.SubtitleText
                textStyle.fontWeight: FontWeight.W100
            }
        }
    }
    
    onSharedFolderIdChanged: {
        sharedFolder.visible = sharedFolderId !== "";
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
    
    attachedObjects: [
        ComponentDefinition {
            id: dirView
            ImageView {
                imageSource: "asset:///images/ic_folder.png"
                filterColor: ui.palette.primary
                scalingMethod: ScalingMethod.AspectFill
                opacity: 0.25
                preferredWidth: ui.du(11)
                preferredHeight: ui.du(11)
            }
        },
        
        ComponentDefinition {
            id: fileView
            FileImageView {
                path: root.pathDisplay
                preferredWidth: ui.du(11)
                preferredHeight: ui.du(11)
            }
        },
        
        ComponentDefinition {
            id: thumbnailView
            ThumbnailImageView {
                size: "w128h128"
                scalingMethod: ScalingMethod.AspectFill
                
                preferredWidth: ui.du(11)
                preferredHeight: ui.du(11)
            }
        }
    ]
}
