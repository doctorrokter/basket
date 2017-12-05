import bb.cascades 1.4

TitleBar {
    id: root
    
    property string value: ""
    
    signal cancel();
    signal typing(string text);
    signal submit(string text);
    
    appearance: TitleBarAppearance.Plain
    kind: TitleBarKind.FreeForm
    
    kindProperties: FreeFormTitleBarKindProperties {
        Container {
            leftPadding: ui.du(1.5)
            rightPadding: ui.du(1.5)
            topPadding: ui.du(1.5)
            
            layout: StackLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            
            
            TextField {
                id: tasksInputField
                
                backgroundVisible: false
                inputMode: TextFieldInputMode.Text
                
                text: root.value
                
                input {
                    keyLayout: KeyLayout.Text
                    submitKey: SubmitKey.EnterKey
                    onSubmitted: {
                        root.submitText();
                    }
                }
                
                onTextChanging: {
                    root.typing(text);
                }
            }
            
            Button {
                text: "X"
                maxWidth: ui.du(5)
                
                onClicked: {
                    root.cancel();
                }
            }
        }
    }
    
    function submitText() {
        root.submit(tasksInputField.text.trim());
    }
    
    function focus() {
        tasksInputField.requestFocus();
    }
    
    function reset() {
        tasksInputField.resetText();
    }
}