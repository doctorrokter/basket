import bb.cascades 1.4
import bb.system 1.2
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
                
                ActionItem {
                    id: propsAction
                    title: qsTr("Properties") + Retranslate.onLocaleOrLanguageChanged
                    imageSource: "asset:///images/ic_properties.png"
                    
                    shortcuts: [
                        Shortcut {
                            key: "p"
                            
                            onTriggered: {
                                propsAction.triggered();
                            }
                        }
                    ]
                },
                
                ActionItem {
                    id: publishAction
                    title: {
                        
                        // TODO: fix this!!!
                        if (true) {
                            return qsTr("Unpublish") + Retranslate.onLocaleOrLanguageChanged;
                        }
                        return qsTr("Publish") + Retranslate.onLocaleOrLanguageChanged;
                    }
                    imageSource: "asset:///images/ic_share.png"
                    
                    shortcuts: [
                        Shortcut {
                            key: "s"
                            
                            onTriggered: {
                                publishAction.triggered();
                            }
                        }
                    ]
                },
                
                ActionItem {
                    id: downloadAction
                    title: qsTr("Download") + Retranslate.onLocaleOrLanguageChanged
                    imageSource: "asset:///images/ic_download.png"
                    enabled: !root.isDir()
                }
            ]
        }
    ]
    
    function isDir() {
        return root.tag === "folder";
    }
    
    function getImage() {
        if (!root.isDir()) {
            var ext = _file.extension(root.name).toLowerCase();
            if (_file.isImage(ext)) {
//                if (root.previewPath !== undefined) {
//                    return root.previewPath;
//                }
                return "asset:///images/ic_doctype_picture.png";
            } else if (_file.isVideo(ext)) {
                return "asset:///images/ic_doctype_video.png";
            } else if (_file.isAudio(ext)) {
                return "asset:///images/ic_doctype_music.png";
            } else if (_file.isPdf(ext)) {
                return "asset:///images/ic_doctype_pdf.png";
            } else if (_file.isDoc(ext)) {
                return "asset:///images/ic_doctype_doc.png";
            } else if (_file.isSpreadSheet(ext)) {
                return "asset:///images/ic_doctype_xls.png";
            } else if (_file.isPresentation(ext)) {
                return "asset:///images/ic_doctype_ppt.png";
            } else {
                return "asset:///images/ic_doctype_generic.png";
            }
        }
        return "asset:///images/ic_folder.png";
    }
    
    function filterColor() {
        if (!root.isDir()) {
//            if (root.previewPath === undefined) {
                return ui.palette.textOnPlain;
//            } else {
//                return 0;
//            }
        }
        return ui.palette.primary;
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
            
            ImageView {
                imageSource: root.getImage();
                filterColor: root.filterColor();
                
                opacity: root.isDir() ? 0.25 : 1.0
                preferredWidth: ui.du(11)
                preferredHeight: ui.du(11)
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
            
            ImageView {
                visible: !root.isDir()
                imageSource: "asset:///images/opac_bg.png"
                
                opacity: 0.5
                preferredWidth: ui.du(11)
                preferredHeight: ui.du(11)
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
                    verticalAlignment: VerticalAlignment.Top
                    
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 1
                    }
                }
                
//                ImageView {
//                    horizontalAlignment: HorizontalAlignment.Right
//                    imageSource: "asset:///images/Splat_96x96.png"
//                    maxWidth: ui.du(5)
//                    maxHeight: ui.du(5)
//                }
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
}
