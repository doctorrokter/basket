import bb.cascades 1.4

Container {
    id: root
    
    property variant textColor
    
    horizontalAlignment: HorizontalAlignment.Center
    verticalAlignment: VerticalAlignment.Bottom
    Label {
        text: qsTr("Swipe left") + Retranslate.onLocaleOrLanguageChanged
        textStyle.color: root.textColor
        margin.bottomOffset: ui.du(2)
    }
    
    onCreationCompleted: {
        hide.play();
    }
    
    animations: [
        FadeTransition {
            id: hide
            fromOpacity: 1.0
            toOpacity: 0.5
            repeatCount: 1
            duration: 1000
            
            onEnded: {
                show.play();
            }
        },
        
        FadeTransition {
            id: show
            fromOpacity: 0.5
            toOpacity: 1.0
            repeatCount: 1
            duration: 1000
            
            onEnded: {
                hide.play();
            }
        }
    ]
}
