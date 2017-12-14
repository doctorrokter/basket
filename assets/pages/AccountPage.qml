import bb.cascades 1.4
import "../components"

Page {
    id: root
    
    property variant account: undefined
    property string accountId: ""
    property string displayName: ""
    property string abbreviatedName: ""
    property string email: ""
    property string profilePhotoUrl: ""
    
    ScrollView {
        Container {
            horizontalAlignment: HorizontalAlignment.Fill
            
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                layout: DockLayout {}
                Container {
                    background: ui.palette.primary
                    minHeight: ui.du(25)
                    horizontalAlignment: HorizontalAlignment.Fill
                    opacity: 0.75
                }
                
                Avatar {
                    id: avatar
                    url: root.profilePhotoUrl
                    abbreviatedName: root.abbreviatedName
                    horizontalAlignment: HorizontalAlignment.Center
                    verticalAlignment: VerticalAlignment.Bottom
                    margin.topOffset: ui.du(18)
                    squared: false
                    circleOpacity: 1.0
                    randomColor: false
                }
            }
            
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                Label {
                    text: root.displayName
                    textStyle.base: SystemDefaults.TextStyles.PrimaryText
                    horizontalAlignment: HorizontalAlignment.Center
                }
                
                Label {
                    text: '<a href="mailto:' + root.email + '">' + root.email + '</a>'
                    textStyle.base: SystemDefaults.TextStyles.SubtitleText
                    textStyle.fontWeight: FontWeight.W300
                    textFormat: TextFormat.Html
                    horizontalAlignment: HorizontalAlignment.Center
                }
            }
            
            Divider {}
        }
    }
    
    onAccountChanged: {
        if (account) {
            root.displayName = root.account.name.display_name;
            root.abbreviatedName = root.account.name.abbreviated_name;
            root.email = root.account.email;
            root.profilePhotoUrl = root.account.profile_photo_url;
        }
    }
}
