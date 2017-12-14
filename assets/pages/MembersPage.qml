import bb.cascades 1.4
import WebImageView 1.0
import "../components"

Page {
    id: root
    
    property string path: ""
    property string name: ""
    property string sharedFolderId: ""
    property variant members: []
    
    signal showAccount(variant account)
    
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
            
            onTriggered: {
                root.showAccount(dataModel.data(indexPath));
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
                            
                            Avatar {
                                url: ListItemData.profile_photo_url
                                abbreviatedName: ListItemData.name.abbreviated_name
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
