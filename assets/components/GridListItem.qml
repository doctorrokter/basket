import bb.cascades 1.4
import bb.device 1.4
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
    
    property string previewPath: undefined
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
    
    function getImage() {
        if (!root.isDir()) {
            var ext = _file.extension(root.name).toLowerCase();
            if (_file.isImage(ext)) {
//                if (root.previewPath) {
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
        if (!root.previewPath) {
            if (!root.isDir()) {
                return ui.palette.textOnPlain;
            }
            return ui.palette.primary;
        }
        return 0;
    }
    
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        background: ui.palette.plain
        
        layout: DockLayout {}
        
        ImageView {
            id: preview
            imageSource: root.getImage();
            filterColor: root.filterColor();
            
            opacity: root.isDir() ? 0.25 : 1.0
            preferredWidth: root.isDir() || (root.previewPath !== undefined) ? listItemLUH.layoutFrame.width : ui.du(20)
            preferredHeight: {
                if (root.dir) {
                    return listItemLUH.layoutFrame.height - ui.du(2);
                } else if (root.previewPath !== undefined) {
                    return listItemLUH.layoutFrame.height;
                } else {
                    return ui.du(20);
                } 
            }
            verticalAlignment: {
                if (root.isDir()) {
                    return VerticalAlignment.Bottom;
                } else if (root.previewPath !== undefined) {
                    return VerticalAlignment.Fill;
                } else {
                    return VerticalAlignment.Center;
                }
            }
            horizontalAlignment: root.isDir() ? HorizontalAlignment.Fill : HorizontalAlignment.Center
        }
        
        ImageView {
            visible: !root.isDir() && root.previewPath === undefined
            imageSource: "asset:///images/opac_bg.png"
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            opacity: 0.5
        }
        
        Container {
            visible: !root.isDir() && root.previewPath !== undefined
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Bottom
            
            background: Color.Black
            preferredHeight: ui.du(8)
            
            opacity: 0.5
        }
        
        Label {
            verticalAlignment: VerticalAlignment.Bottom
            text: root.name
            textStyle.base: SystemDefaults.TextStyles.SubtitleText
            
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
        
//        ImageView {
//            horizontalAlignment: HorizontalAlignment.Right
//            verticalAlignment: VerticalAlignment.Top
//            imageSource: "asset:///images/Splat_96x96.png"
//            maxWidth: ui.du(5)
//            maxHeight: ui.du(5)
//        }
        
        attachedObjects: [
            LayoutUpdateHandler {
                id: listItemLUH
            }
        ]
    }
    
    attachedObjects: [
        DisplayInfo {
            id: displayInfo
        }
    ]
    
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
                        if (root.publicUrl) {
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
}
