import bb.cascades 1.4
import "../components"

Sheet {
    id: root
    
    property string path: ""
    property string sharedFolderId: ""
    property variant memberAccount: undefined
    property bool createMode: false
    
    Page {
        
        titleBar: TitleBar {
            title: {
                if (root.createMode) {
                    return qsTr("Add member") + Retranslate.onLocaleOrLanguageChanged;
                }
                return qsTr("Edit member") + Retranslate.onLocaleOrLanguageChanged;
            }
            
            dismissAction: ActionItem {
                title: qsTr("Cancel") + Retranslate.onLocaleOrLanguageChanged
                
                onTriggered: {
                    root.close();
                }
            }
            
            acceptAction: ActionItem {
                id: doneAction
                title: {
                    if (root.createMode) {
                        return qsTr("Done") + Retranslate.onLocaleOrLanguageChanged;
                    }
                    return qsTr("Save") + Retranslate.onLocaleOrLanguageChanged;
                }
                
                onTriggered: {
                    spinner.start();
                    if (root.createMode) {
                        _qdropbox.addFolderMember(root.sharedFolderId, memberName.text.trim().split(" "), accessLevel.selectedOption.value);
                    } else {
                        _qdropbox.updateFolderMember(root.sharedFolderId, root.memberAccount, accessLevel.selectedOption.value);
                    }
                }
                
                enabled: memberName.text !== ""
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
                            text: {
                                if (root.createMode) {
                                    return qsTr("To (use space as delimeter):") + Retranslate.onLocaleOrLanguageChanged
                                }
                                return qsTr("User to edit:") + Retranslate.onLocaleOrLanguageChanged;
                            }
                        }
                    }
                    
                    Container {
                        TextArea {
                            id: memberName
                            enabled: root.createMode
                            hintText: {
                                if (root.createMode) {
                                    return qsTr("Ex.: e1@gmail.com e2@gmail.com") + Retranslate.onLocaleOrLanguageChanged
                                }
                                return "";
                            }
                            
                            onTextChanging: {
                                doneAction.enabled = text.trim() !== "";
                            }
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
    
    function folderMemberAdded() {
        spinner.stop();
        root.close();
    }
    
    onCreationCompleted: {
        _qdropbox.folderMemberUpdated.connect(root.folderMemberUpdated);
        _qdropbox.folderMemberAdded.connect(root.folderMemberAdded);
    }
    
    onMemberAccountChanged: {
        if (memberAccount) {
            memberName.text = root.memberAccount.name.display_name + " (" + root.memberAccount.email + ")";
        } else {
            memberName.resetText();
        }
    }
    
    onOpened: {
        if (root.createMode) {
            memberName.requestFocus();
        }
    }
    
    onClosed: {
        memberName.resetText();
        root.createMode = false;
        root.memberAccount = undefined;
    }
}