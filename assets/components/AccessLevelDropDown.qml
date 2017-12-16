import bb.cascades 1.4

Container {
    
    property Option selectedOption: accessLevel.selectedOption
    
    leftPadding: ui.du(2)
    topPadding: ui.du(4)
    rightPadding: ui.du(2)
    bottomPadding: ui.du(2)
    
    DropDown {
        id: accessLevel
        title: qsTr("Access level") + Retranslate.onLocaleOrLanguageChanged
        
        options: [
//            Option {
//                id: ownerOption
//                text: qsTr("Owner") + Retranslate.onLocaleOrLanguageChanged
//                value: 0
//            },
            
            Option {
                id: editorOption
                text: qsTr("Editor") + Retranslate.onLocaleOrLanguageChanged
                value: 1
            },
            
            Option {
                id: viewerOption
                text: qsTr("Viewer") + Retranslate.onLocaleOrLanguageChanged
                value: 2
                selected: true
            }
            
//            Option {
//                id: viewerNoCommentOption
//                text: qsTr("Viewer no comment") + Retranslate.onLocaleOrLanguageChanged
//                value: 3
//            }
        ]
    }
    
    Container {
//        Label {
//            id: ownerLabel
//            text: qsTr("The collaborator is the owner of the shared folder. Owners can view and edit the shared folder.") + Retranslate.onLocaleOrLanguageChanged
//            multiline: true
//            visible: ownerOption.selected
//            textStyle.fontWeight: FontWeight.W300
//        }
        
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
        
//        Label {
//            id: viewerNoCommentLabel
//            text: qsTr("Users can only view the shared folder and does not have any access to comments.") + Retranslate.onLocaleOrLanguageChanged
//            multiline: true
//            visible: viewerNoCommentOption.selected
//            textStyle.fontWeight: FontWeight.W300
//        }
    }
}
