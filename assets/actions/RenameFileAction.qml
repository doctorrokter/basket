import bb.cascades 1.4
import bb.system 1.2

ActionItem {
    id: renameAction
    
    property string name: ""
    property string path: ""
    property bool isDir: true
    
    title: qsTr("Rename") + Retranslate.onLocaleOrLanguageChanged
    imageSource: "asset:///images/ic_rename.png"
    
    onTriggered: {
        if (!isDir) {
            var ext = "." + _file.extension(renameAction.name).toLowerCase();
            var regEx = new RegExp(ext, "ig");
            var name = renameAction.name.replace(regEx, "");
            renamePrompt.inputField.defaultText = name;
            renamePrompt.extenstion = ext;
        } else {
            renamePrompt.inputField.defaultText = renameAction.name;
        }
        renamePrompt.show();
    }
    
    shortcuts: [
        SystemShortcut {
            type: SystemShortcuts.Edit
            
            onTriggered: {
                renameAction.triggered();
            }
        }
    ]
    
    attachedObjects: [
        SystemPrompt {
            id: renamePrompt
            
            property string extenstion: ""
            
            title: qsTr("Enter new name") + Retranslate.onLocaleOrLanguageChanged
            dismissAutomatically: true
            
            onFinished: {
                if (value === 2) {
                    var newName = renamePrompt.inputFieldTextEntry() + extenstion;
                    _qdropbox.rename(renameAction.path, renameAction.path.replace(renameAction.name, newName));
                }
            }    
        }
    ]
}
