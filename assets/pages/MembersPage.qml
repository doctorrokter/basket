import bb.cascades 1.4
import WebImageView 1.0
import "../components"

Page {
    id: root
    
    property string path: ""
    property string name: ""
    property string sharedFolderId: ""
    property variant members: []
    property bool isOwner: false
    
    signal showAccount(variant account)
    
    titleBar: TitleBar {
        title: qsTr("Members") + Retranslate.onLocaleOrLanguageChanged
    }
    
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        
        layout: DockLayout {}
        
        Container {
            Header {
                title: root.path
            }
            
            ListView {
                id: listView
                scrollRole: ScrollRole.Main
                
                dataModel: ArrayDataModel {
                    id: dataModel
                }
                
                onTriggered: {
                    root.showAccount(dataModel.data(indexPath));
                }
                
                contextActions: [
                    ActionSet {
                        DeleteActionItem {
                            id: removeFolderMember
                            enabled: root.isOwner
                            
                            onTriggered: {
                                var member = dataModel.data(listView.selected());
                                _qdropbox.removeFolderMember(root.sharedFolderId, member);
                            }
                            
                            shortcuts: Shortcut {
                                key: "d"
                                
                                onTriggered: {
                                    removeFolderMember.triggered();
                                }
                            }
                        }
                    }
                ]
                
                listItemComponents: [
                    ListItemComponent {
                        CustomListItem {
                            id: accountItem
                            
                            property ListView listView: ListItem.view
                            
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
        
        ActivityIndicator {
            id: spinner
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            minWidth: ui.du(20)
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
        spinner.stop();
    }
    
    function folderMemberRemoved(sharedFolderId, member) {
        if (root.sharedFolderId === sharedFolderId) {
            for (var i = 0; i < dataModel.size(); i++) {
                var m = dataModel.value(i);
                if (m.email === member.email || m.dropbox_id === member.dropbox_id) {
                    dataModel.removeAt(i);
                    return;
                }
            }
        }
    }
    
    function cleanUp() {
        _qdropbox.listFolderMembersLoaded.disconnect(root.listFolderMembersLoaded);
        _qdropbox.accountBatchLoaded.disconnect(root.accountBatchLoaded);
        _qdropbox.folderMemberRemoved.disconnect(root.folderMemberRemoved);
    }
    
    onCreationCompleted: {
        _qdropbox.listFolderMembersLoaded.connect(root.listFolderMembersLoaded);
        _qdropbox.accountBatchLoaded.connect(root.accountBatchLoaded);
        _qdropbox.folderMemberRemoved.connect(root.folderMemberRemoved);
    }
    
    onSharedFolderIdChanged: {
        spinner.start();
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
