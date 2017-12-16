import bb.cascades 1.4
import bb.system 1.2
import WebImageView 1.0
import basket.helpers 1.0
import "../components"

Page {
    id: root
    
    property string path: ""
    property string name: ""
    property string sharedFolderId: ""
    property variant members: []
    property bool isOwner: false
    property variant indexPath: []
    property variant memberToRemove: undefined
    
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
                                root.indexPath = listView.selected();
                                root.memberToRemove = dataModel.data(indexPath);
                                root.removeMember(root.memberToRemove);
                                toast.body = (qsTr("Member removed") + Retranslate.onLocaleOrLanguageChanged) + ": " +  root.memberToRemove.name.display_name;
                                toast.show();
                                timer.start();
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
    
    attachedObjects: [
        SystemToast {
            id: toast
            
            position: SystemUiPosition.BottomCenter
            button.enabled: true
            button.label: qsTr("Undo") + Retranslate.onLocaleOrLanguageChanged
            
            onFinished: {
                if (value !== 1) {
                    _qdropbox.removeFolderMember(root.sharedFolderId, root.memberToRemove);
                } else {
                    timer.stop();
                    dataModel.insert(root.indexPath[0], root.memberToRemove);
                    root.memberToRemove = undefined;
                    root.indexPath = [];
                }
            }
        },
        
        Timer {
            id: timer
            
            interval: 2000
            singleShot: true
            
            onTimeout: {
                toast.finished(SystemUiResult.TimeOut);
                toast.destroy();
            }
        }
    ]
    
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
    
    function removeMember(member) {
        for (var i = 0; i < dataModel.size(); i++) {
            var m = dataModel.value(i);
            if (m.email === member.email || m.dropbox_id === member.dropbox_id) {
                dataModel.removeAt(i);
                return;
            }
        }
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
