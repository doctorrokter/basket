import bb.cascades 1.4

Container {
    id: root
    
    property string imageSource: ""
    property double imageOpacity: 1.0
    
    layout: DockLayout {}
    
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Center
    
    background: ui.palette.plainBase
    
    ImageView {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        scalingMethod: ScalingMethod.AspectFill
        imageSource: root.imageSource
        opacity: root.imageOpacity
    }
}
