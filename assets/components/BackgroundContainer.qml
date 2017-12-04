import bb.cascades 1.4

Container {
    id: root
    
    property string imageSource: ""
    
    layout: DockLayout {}
    
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Center
    
    background: ui.palette.plainBase
    
    ImageView {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        scalingMethod: ScalingMethod.AspectFill
        imageSource: root.imageSource
    }
}
