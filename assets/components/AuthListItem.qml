import bb.cascades 1.4
import bb.device 1.4

CustomListItem {
    id: root
    
    property int screenWidth: 1440
    property int screenHeight: 1440
    property string title: ""
    property string imageSource: ""
    property string description: ""
    property string color: ""
    property variant textColor
    property bool last: false
    property string inputText: ""
    property string inputHintText: ""
    property bool inputBackground: false
    
    signal done(string token)
    
    highlightAppearance: HighlightAppearance.None
    
    preferredWidth: ListItem.view.width
    preferredHeight: ListItem.view.height
    
    ScrollView {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        ColorContainer {
            color: root.color
            
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                margin.topOffset: ui.du(5)
                Label {
                    text: root.title
                    horizontalAlignment: HorizontalAlignment.Center
                    textStyle.base: SystemDefaults.TextStyles.BigText
                    textStyle.color: root.textColor
                    multiline: true
                }
                
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    ImageView {
                        horizontalAlignment: HorizontalAlignment.Center
                        imageSource: root.imageSource
                        scalingMethod: ScalingMethod.AspectFit
                        margin.leftOffset: ui.du(5)
                        margin.rightOffset: ui.du(5)
                        maxWidth: {
                            if (root.deviceIsSmall()) {
                                return ui.du(35);
                            } else if (root.deviceIsBig()) {
                                return ui.du(70);
                            } else {
                                return ui.du(65);
                            }
                        }
                    }
                }
                
                Label {
                    horizontalAlignment: HorizontalAlignment.Center
                    text: root.description
                    multiline: true
                    textStyle.color: root.textColor
                    textFormat: TextFormat.Html
                }
                
                TextArea {
                    id: textArea
                    text: root.inputText
                    visible: root.inputText !== "" || root.inputHintText !== ""
                    hintText: root.inputHintText
                    backgroundVisible: root.inputBackground
                
                }
            }
            
            SwipeContainer {
                visible: !root.last    
                textColor: root.textColor
            }
            
            Button {
                id: doneButton
                visible: root.last
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Bottom
                margin.bottomOffset: ui.du(2)
                text: qsTr("Done!") + Retranslate.onLocaleOrLanguageChanged
                enabled: textArea.text.trim() !== ""
                
                onClicked: {
                    root.done(textArea.text.trim());
                }
            }
        }
    }
    
    function deviceIsSmall() {
        return root.screenWidth === 720 && root.screenHeight === 720;
    }
    
    function deviceIsBig() {
        return root.screenWidth === 1440 && root.screenHeight === 1440;
    }
    
    attachedObjects: [
        DisplayInfo {
            id: display
        }
    ]
    
    onCreationCompleted: {
        root.screenWidth = display.pixelSize.width;
        root.screenHeight = display.pixelSize.height;
    }
}