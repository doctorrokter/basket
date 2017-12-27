import bb.cascades 1.4
import "../components"

NavigationPane {
    
    id: navPane
    
    Page {
        id: root
        
        BackgroundContainer {
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            
            imageSource: "asset:///splash/splash_1440_x_1440.png"
            
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Bottom
                margin.bottomOffset: ui.du(15)
                Button {
                    text: qsTr("Sign in") + Retranslate.onLocaleOrLanguageChanged
                    
                    onClicked: {
//                        _app.authorize();
                        var ap = authFlowPage.createObject();
                        navPane.push(ap);
                    }
                }
            }
        }
    }
    
    attachedObjects: [
        ComponentDefinition {
            id: authFlowPage
            AuthFlowPage {
                
            }
        }
    ]
}
