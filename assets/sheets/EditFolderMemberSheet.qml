import bb.cascades 1.4
import "../components"

Sheet {
    id: root
    
    property string path: ""
    property string sharedFolderId: ""
    property variant memberAccount: undefined
    
    Page {
        
        titleBar: TitleBar {
            title: qsTr("Edit member") + Retranslate.onLocaleOrLanguageChanged
            
            dismissAction: ActionItem {
                title: qsTr("Cancel") + Retranslate.onLocaleOrLanguageChanged
                
                onTriggered: {
                    root.close();
                }
            }
            
            acceptAction: ActionItem {
                title: qsTr("Save") + Retranslate.onLocaleOrLanguageChanged
                
                onTriggered: {
                    spinner.start();
                    _qdropbox.updateFolderMember(root.sharedFolderId, root.memberAccount, accessLevel.selectedOption.value);
                }
            }
        }
        
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            
            layout: DockLayout {}
            
            ScrollView {
                scrollRole: ScrollRole.Main
                
                Container {
                    
                    Container {
                        leftPadding: ui.du(2)
                        topPadding: ui.du(2)
                        rightPadding: ui.du(2)
                        bottomPadding: ui.du(2)
                        
                        Label {
                            text: qsTr("Resource to share:") + Retranslate.onLocaleOrLanguageChanged
                        }
                    }
                    
                    Container {
                        TextField {
                            text: root.path
                            enabled: false
                        }
                    }
                    
                    Container {
                        leftPadding: ui.du(2)
                        topPadding: ui.du(2)
                        rightPadding: ui.du(2)
                        bottomPadding: ui.du(2)
                        
                        Label {
                            text: qsTr("User to edit:") + Retranslate.onLocaleOrLanguageChanged
                        }
                    }
                    
                    Container {
                        TextField {
                            id: memberName
                            enabled: false
                        }
                    }
                    
                    AccessLevelDropDown {
                        id: accessLevel
                    }
                }
            }
            
            ActivityIndicator {
                id: spinner
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Center
                minWidth: ui.du(20)
            }
        }
    }
    
    function folderMemberUpdated(sharedFolderId, member) {
        spinner.stop();
        root.memberAccount = undefined;
        root.close();
    }
    
    onCreationCompleted: {
        _qdropbox.folderMemberUpdated.connect(root.folderMemberUpdated);
    }
    
    onMemberAccountChanged: {
        if (memberAccount) {
            memberName.text = root.memberAccount.name.display_name + " (" + root.memberAccount.email + ")";
        }
    }
}