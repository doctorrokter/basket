import bb.cascades 1.4
import "../components"

Page {
    
    id: root
    
    property variant steps: {
        FIRST: "FIRST",
        SECOND: "SECOND",
        THIRD: "THIRD",
        FOURTH: "FOURTH",
        FIFTH: "FIFTH"
    }
    
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        layout: DockLayout {}
        
        background: Color.White
        
        ListView {
            id: listView
            
            property double width: 0
            property double height: 0
            
            dataModel: ArrayDataModel {
                id: dataModel
            }
            
            layout: StackListLayout {
                orientation: LayoutOrientation.LeftToRight
            }
            
            flickMode: FlickMode.SingleItem
            
            function itemType(data, indexPath) {
                return data.step;
            }
            
            listItemComponents: [
                ListItemComponent {
                    type: root.steps.FIRST
                    AuthListItem {
                        color: "#F0F0F0"
                        title: qsTr("Navigate to a modern browser") + Retranslate.onLocaleOrLanguageChanged
                        description: qsTr("Since the core BB10 browser does not support some new security policies there is no methods to authorize in Dropbox easily.\n\nCopy this link and open it in any modern browser (mobile or desktop):") + Retranslate.onLocaleOrLanguageChanged
                        inputText: "https://www.dropbox.com/oauth2/authorize?client_id=q2ficti4tr8zql8&redirect_uri=http://localhost:8080/auth&response_type=token"
                        textColor: Color.Black
                    }
                },
                
                ListItemComponent {
                    type: root.steps.SECOND
                    AuthListItem {
                        color: "#087099"
                        title: qsTr("Complete the sign in flow in Dropbox") + Retranslate.onLocaleOrLanguageChanged
                        imageSource: "asset:///images/auth.png"
                        textColor: Color.White
                    }
                },
                
                ListItemComponent {
                    type: root.steps.THIRD
                    AuthListItem {
                        color: "#CC3333"
                        title: qsTr("Allow BasketBB10 app use your Dropbox") + Retranslate.onLocaleOrLanguageChanged
                        imageSource: "asset:///images/grant.png"
                        textColor: Color.White
                    }
                },
                
                ListItemComponent {
                    type: root.steps.FOURTH
                    AuthListItem {
                        color: "#DCD427"
                        title: qsTr("Retrieve auth token from generated URL") + Retranslate.onLocaleOrLanguageChanged
                        imageSource: "asset:///images/token.png"
                        textColor: Color.Black
                        description: qsTr("Copy ONLY access token. You will be redirected to a URL which will look like: ") + Retranslate.onLocaleOrLanguageChanged + 
                            "http://localhost:8080/auth#access_token={YOUR_TOKEN_HERE}&token_type=bearer&uid={YOUR_UID}&account_id={YOUR_ACCOUNT_ID}"
                    }
                },
                
                ListItemComponent {
                    type: root.steps.FIFTH
                    AuthListItem {
                        color: "#779933"
                        title: qsTr("You are almost here!") + Retranslate.onLocaleOrLanguageChanged
                        textColor: Color.Black
                        description: qsTr("Paste your token in the input below:") + Retranslate.onLocaleOrLanguageChanged
                        last: true
                        inputHintText: "Your token here"
                        
                        onDone: {
                            _app.auth(token);
                        }
                    }
                }
            ]
            
            attachedObjects: [
                LayoutUpdateHandler {
                    id: rootLUH
                    
                    onLayoutFrameChanged: {
                        listView.width = layoutFrame.width;
                        listView.height = layoutFrame.height;
                    }
                }
            ]
        }   
    }
    
    onCreationCompleted: {
        var data = [];
        data.push({step: root.steps.FIRST});
        data.push({step: root.steps.SECOND});
        data.push({step: root.steps.THIRD});
        data.push({step: root.steps.FOURTH});
        data.push({step: root.steps.FIFTH});
        dataModel.append(data);
    }
}
