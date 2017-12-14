import bb.cascades 1.4
import WebImageView 1.0

Page {
    id: root
    
    property string path: ""
    property string name: ""
    property string sharedFolderId: ""
    property variant members: []
    
    titleBar: TitleBar {
        title: qsTr("Members") + Retranslate.onLocaleOrLanguageChanged
    }
    
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        
        Header {
            title: root.path
        }
        
        ListView {
            id: listView
            
            dataModel: ArrayDataModel {
                id: dataModel
            }
            
            listItemComponents: [
                ListItemComponent {
                    CustomListItem {
                        id: accountItem
                        
                        function hasPhoto() {
                            return ListItemData.profile_photo_url !== undefined && ListItemData.profile_photo_url !== "";
                        }          
                        
                        Container {
                            layout: StackLayout {
                                orientation: LayoutOrientation.LeftToRight
                            }
                            
                            leftPadding: ui.du(1)
                            topPadding: ui.du(1)
                            rightPadding: ui.du(1)
                            bottomPadding: ui.du(1)
                            
                            Container {
                                layout: DockLayout {}
                                WebImageView {
                                    visible: accountItem.hasPhoto()
                                    url: accountItem.hasPhoto() ? ListItemData.profile_photo_url.replace("%3A", ":") : ""
                                    minWidth: ui.du(13)
                                    minHeight: ui.du(13)
                                    maxWidth: ui.du(13)
                                    maxHeight: ui.du(13)
                                }
                                
                                ImageView {
                                    visible: accountItem.hasPhoto()
                                    imageSource: "asset:///images/transparent_circle.png"
                                    filterColor: ui.palette.background
                                    minWidth: ui.du(13)
                                    minHeight: ui.du(13)
                                    maxWidth: ui.du(13)
                                    maxHeight: ui.du(13)
                                }
                                
                                Container {
                                    visible: !accountItem.hasPhoto()
                                    minWidth: ui.du(13)
                                    minHeight: ui.du(13)
                                    maxWidth: ui.du(13)
                                    maxHeight: ui.du(13)
                                    
                                    layout: DockLayout {}
                                    
                                    ImageView {
                                        id: circleImage
                                        filterColor: Color.create(_app.getRandomColor())
                                        imageSource: "asset:///images/circle.png"
                                        horizontalAlignment: HorizontalAlignment.Fill
                                        verticalAlignment: VerticalAlignment.Fill
                                        opacity: 0.75
                                    }
                                    
                                    Label {
                                        text: ListItemData.name.abbreviated_name
                                        horizontalAlignment: HorizontalAlignment.Center
                                        verticalAlignment: VerticalAlignment.Center
                                        textStyle.color: Color.White
                                    }
                                }
                            }
                            
                            Container {
                                leftPadding: ui.du(2)
                                
                                Container {
                                    Label {
                                        text: ListItemData.name.display_name
                                        textStyle.base: SystemDefaults.TextStyles.PrimaryText
                                    }
                                }
                                                                   
                                Container {
                                    Label {
                                        text: ListItemData.email
                                        textStyle.base: SystemDefaults.TextStyles.SubtitleText
                                    }
                                }
                                
                                Container {
                                    Label {
                                        text: ListItemData.access_type
                                        textStyle.base: SystemDefaults.TextStyles.SubtitleText
                                        textStyle.color: ui.palette.secondaryTextOnPlain
                                        textStyle.fontWeight: FontWeight.W300
                                    }
                                }
                            }
                        }
                    }
                }
            ]
        }
    }
    
    function listFolderMembersLoaded(sharedFolderId, members, cursor) {
        root.members = members;
    }
    
    function accountBatchLoaded(accounts) {
        accounts.forEach(function(a) {
            root.members.forEach(function(m) {
                if (m.user.account_id === a.account_id) {
                    a.access_type = m.access_type[".tag"];
                    return;
                }    
            });   
        });
        dataModel.append(accounts);
    }
    
    function cleanUp() {
        _qdropbox.listFolderMembersLoaded.disconnect(root.listFolderMembersLoaded);
        _qdropbox.accountBatchLoaded.disconnect(root.accountBatchLoaded);
    }
    
    onCreationCompleted: {
        _qdropbox.listFolderMembersLoaded.connect(root.listFolderMembersLoaded);
        _qdropbox.accountBatchLoaded.connect(root.accountBatchLoaded);
    }
    
    onSharedFolderIdChanged: {
        _qdropbox.listFolderMembers(sharedFolderId);
    }
    
    onMembersChanged: {
        if (members.length > 0) {
            var ids = members.map(function(m) {
                    return m.user.account_id;    
            });
            _qdropbox.getAccountBatch(ids);
        }
    }
}
