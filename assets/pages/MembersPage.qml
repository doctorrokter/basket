import bb.cascades 1.4
import bb.system 1.2
import WebImageView 1.0
import basket.helpers 1.0
import "../components"
import "../sheets"

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
    
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    actionBarVisibility: ChromeVisibility.Overlay
    
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
                        actions: [
                            DeleteActionItem {
                                id: removeFolderMember
                                enabled: root.isOwner
                                
                                onTriggered: {
                                    root.indexPath = listView.selected();
                                    root.memberToRemove = dataModel.data(root.indexPath);
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
                            },
                            
                            ActionItem {
                                id: updateFolderMember
                                enabled: root.isOwner
                                imageSource: "asset:///images/ic_edit.png"
                                title: qsTr("Edit") + Retranslate.onLocaleOrLanguageChanged
                                
                                onTriggered: {
                                    editFolderMemberSheet.memberAccount = dataModel.data(listView.selected());
                                    editFolderMemberSheet.open();
                                }
                                
                                shortcuts: Shortcut {
                                    key: "e"
                                    
                                    onTriggered: {
                                        removeFolderMember.triggered();
                                    }
                                }
                            }
                        ]
                    }
                ]
                
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
        
        ActivityIndicator {
            id: spinner
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            minWidth: ui.du(20)
        }
    }
    
    actions: [
        ActionItem {
            id: addMember
            title: qsTr("Add member") + Retranslate.onLocaleOrLanguageChanged
            imageSource: "asset:///images/ic_add.png"
            ActionBar.placement: ActionBarPlacement.Signature
            
            onTriggered: {
                editFolderMemberSheet.createMode = true;
                editFolderMemberSheet.open();
            }
            
            shortcuts: Shortcut {
                key: "c"
                
                onTriggered: {
                    addMember.triggered();
                }
            }
        }
    ]
    
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
        },
        
        EditFolderMemberSheet {
            id: editFolderMemberSheet
            
            path: root.path
            sharedFolderId: root.sharedFolderId
        }
    ]
    
    function listFolderMembersLoaded(sharedFolderId, members, cursor) {
        root.members = members;
    }
    
    function accountBatchLoaded(accounts) {
        dataModel.clear();
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
            if (m.email === member.email || m.account_id === member.account_id) {
                dataModel.removeAt(i);
                return;
            }
        }
    }
    
    function folderMemberUpdated(sharedFolderId, member) {
        if (root.sharedFolderId === sharedFolderId) {
            for (var i = 0; i < dataModel.size(); i++) {
                var m = dataModel.value(i);
                if (m.account_id === member.member.dropbox_id || m.email === member.member.email) {
                    m.access_type = member.access_level;
                    dataModel.replace(i, m);
                    return;
                }
            }
        }
    }
    
    function cleanUp() {
        _qdropbox.listFolderMembersLoaded.disconnect(root.listFolderMembersLoaded);
        _qdropbox.accountBatchLoaded.disconnect(root.accountBatchLoaded);
        _qdropbox.folderMemberUpdated.disconnect(root.folderMemberUpdated);
    }
    
    onCreationCompleted: {
        _qdropbox.listFolderMembersLoaded.connect(root.listFolderMembersLoaded);
        _qdropbox.accountBatchLoaded.connect(root.accountBatchLoaded);
        _qdropbox.folderMemberUpdated.connect(root.folderMemberUpdated);
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
