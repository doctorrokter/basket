import bb.cascades 1.4

Sheet {
    id: root
    
    property string path: ""
    
    Page {
        
        titleBar: TitleBar {
            title: qsTr("Share") + Retranslate.onLocaleOrLanguageChanged
            
            dismissAction: ActionItem {
                id: cancelAction
                title: qsTr("Cancel") + Retranslate.onLocaleOrLanguageChanged
                
                onTriggered: {
                    root.path = "";
                    root.close();
                }
            }
            
            acceptAction: ActionItem {
                id: doneAction
                title: qsTr("Done") + Retranslate.onLocaleOrLanguageChanged
                enabled: emailsField.text !== ""
                
                onTriggered: {
                    spinner.start();    
                    _qdropbox.error.connect(root.onError);
                    _qdropbox.sharedFolder.connect(root.addMembers);
                    _qdropbox.shareFolder(root.path);
                }
            }
        }
        
        ScrollView {
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                
                layout: DockLayout {}
                
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
                            text: qsTr("To (use space as delimeter):") + Retranslate.onLocaleOrLanguageChanged
                        }
                    }
                    
                    Container {
                        TextField {
                            id: emailsField
                            hintText: "Ex.: e1@gmail.com e2@gmail.com"
                            
                            onTextChanging: {
                                doneAction.enabled = text.trim() !== ""
                            }
                        }
                    }
                    
                    Container {
                        leftPadding: ui.du(2)
                        topPadding: ui.du(4)
                        rightPadding: ui.du(2)
                        bottomPadding: ui.du(2)
                        
                        DropDown {
                            id: accessLevel
                            title: qsTr("Access level") + Retranslate.onLocaleOrLanguageChanged
                            
                            options: [
                                Option {
                                    id: editorOption
                                    text: qsTr("Editor") + Retranslate.onLocaleOrLanguageChanged
                                    value: 1
                                    selected: true
                                },
                                
                                Option {
                                    id: viewerOption
                                    text: qsTr("Viewer") + Retranslate.onLocaleOrLanguageChanged
                                    value: 2
                                }
                            ]
                        }
                        
                        Container {
                            Label {
                                id: editorLabel
                                text: qsTr("Users can modify, delete and comment a file and also add it into own Dropbox.") + Retranslate.onLocaleOrLanguageChanged
                                multiline: true
                                visible: editorOption.selected
                                textStyle.fontWeight: FontWeight.W300
                            }
                            
                            Label {
                                id: viewerLabel
                                text: qsTr("Users can view, download and comment a file.") + Retranslate.onLocaleOrLanguageChanged
                                multiline: true
                                visible: viewerOption.selected
                                textStyle.fontWeight: FontWeight.W300
                            }
                        }
                    }
                }
                
                ActivityIndicator {
                    id: spinner
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Center
                    minWidth: ui.du(20)
                }
            }
        }
    }
    
    function onError() {
        _qdropbox.error.disconnect(root.onError);
        spinner.stop();
    }
    
    function addMembers(path, sharedFolderId) {
        _qdropbox.sharedFolder.disconnect(root.addMembers);
        _qdropbox.folderMemberAdded.connect(root.folderMemberAdded);
        if (root.path === path) {
            _qdropbox.addFolderMember(sharedFolderId, emailsField.text.trim().split(" "), accessLevel.selectedOption.value);
        }
    }
    
    function folderMemberAdded() {
        _qdropbox.error.disconnect(root.onError);
        _qdropbox.folderMemberAdded.disconnect(root.folderMemberAdded);
        spinner.stop();
        root.close();
    }
}