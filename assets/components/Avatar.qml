import bb.cascades 1.4
import WebImageView 1.0

Container {
    id: root
    
    property string url: ""
    property string abbreviatedName: ""
    property bool squared: true
    property double circleOpacity: 0.75
    property bool randomColor: true
    
    layout: DockLayout {}
    WebImageView {
        visible: hasPhoto()
        url: hasPhoto() ? root.url.replace("%3A", ":") : ""
        minWidth: ui.du(13)
        minHeight: ui.du(13)
        maxWidth: ui.du(13)
        maxHeight: ui.du(13)
    }
    
    ImageView {
        visible: hasPhoto() && root.squared
        imageSource: "asset:///images/transparent_circle.png"
        filterColor: ui.palette.background
        minWidth: ui.du(13)
        minHeight: ui.du(13)
        maxWidth: ui.du(13)
        maxHeight: ui.du(13)
    }
    
    Container {
        visible: !hasPhoto()
        minWidth: ui.du(13)
        minHeight: ui.du(13)
        maxWidth: ui.du(13)
        maxHeight: ui.du(13)
        
        layout: DockLayout {}
        
        ImageView {
            id: circleImage
            filterColor: root.randomColor ? Color.create(_app.getRandomColor()) : ui.palette.primary
            imageSource: "asset:///images/circle.png"
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            opacity: root.circleOpacity
        }
        
        Label {
            text: root.abbreviatedName
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
            textStyle.color: Color.White
        }
    }
    
    function hasPhoto() {
        return root.url !== undefined && root.url !== "";
    }
}
