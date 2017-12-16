import bb.cascades 1.4

Page {
    id: root
    
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    actionBarVisibility: ChromeVisibility.Overlay
    
    titleBar: TitleBar {
        title: qsTr("Settings") + Retranslate.onLocaleOrLanguageChanged
    }
    
    ScrollView {
        scrollRole: ScrollRole.Main
        
        Container {
            layout: DockLayout {}
            Container {
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                
                Header {
                    title: qsTr("Look and Feel") + Retranslate.onLocaleOrLanguageChanged
                }
                
                Container {
                    layout: DockLayout {}
                    topPadding: ui.du(2)
                    bottomPadding: ui.du(2.5)
                    leftPadding: ui.du(2.5)
                    rightPadding: ui.du(2.5)
                    horizontalAlignment: HorizontalAlignment.Fill
                    Label {
                        text: qsTr("Dark theme") + Retranslate.onLocaleOrLanguageChanged
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Left
                    }
                    
                    ToggleButton {
                        id: themeToggle
                        horizontalAlignment: HorizontalAlignment.Right
                        
                        onCheckedChanged: {
                            if (checked) {
                                Application.themeSupport.setVisualStyle(VisualStyle.Dark);
                                _app.prop("theme", "DARK");
                            } else {
                                Application.themeSupport.setVisualStyle(VisualStyle.Bright);
                                _app.prop("theme", "BRIGHT");
                            }
                        }
                    }
                }
                
                Header {
                    title: qsTr("Behavior") + Retranslate.onLocaleOrLanguageChanged
                }
                
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    topPadding: ui.du(2.5)
                    leftPadding: ui.du(2.5)
                    rightPadding: ui.du(2.5)
                    bottomPadding: ui.du(2.5)
                    DropDown {
                        title: qsTr("Date/time format") + Retranslate.onLocaleOrLanguageChanged
                        
                        options: [
                            Option {
                                id: customFormatOption
                                text: "d MMM yyyy, h:mm"
                                value: "d MMM yyyy, h:mm"
                            },
                            
                            Option {
                                id: localizedFormatOption
                                text: qsTr("Localized") + Retranslate.onLocaleOrLanguageChanged
                                value: "localized"
                            }
                        ]
                        
                        onSelectedOptionChanged: {
                            _app.prop("date_format", selectedOption.value);
                        }
                    }
                }
                
                Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    minHeight: ui.du(20)
                }
            }
        }
    }
    
    function adjustTheme() {
        var theme = _app.prop("theme");
        themeToggle.checked = theme && theme === "DARK";
    }
    
    function adjustDateTimeFormat() {
        var df = _app.prop("date_format");
        customFormatOption.selected = (df === "" || df === customFormatOption.value);
        localizedFormatOption.selected = (df === localizedFormatOption.value);
    }
       
    onCreationCompleted: {
        adjustTheme();
        adjustDateTimeFormat();
    }
}