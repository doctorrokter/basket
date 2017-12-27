import bb.cascades 1.4

Container {
    id: root
    
    property string color: ""
    
    horizontalAlignment: HorizontalAlignment.Fill
    verticalAlignment: VerticalAlignment.Fill
    background: Color.create(root.color)
}