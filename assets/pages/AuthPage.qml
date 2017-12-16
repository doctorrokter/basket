import bb.cascades 1.4
import "../components"

Page {
    id: root
    
    ScrollView {
        id: scrollView
        
        scrollRole: ScrollRole.Main
        
        Container {
            layout: DockLayout {}
            
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                
                ImageView {
                    horizontalAlignment: HorizontalAlignment.Center
                    imageSource: "asset:///images/d_logo.png"
                }
                
                Container {
                    horizontalAlignment: HorizontalAlignment.Center
                    Button {
                        text: qsTr("Sign in") + Retranslate.onLocaleOrLanguageChanged
                        
                        onClicked: {
                            _app.authorize();
                        }
                    }
                }
            }
        }
    }
}
