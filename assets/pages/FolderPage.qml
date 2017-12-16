import bb.cascades 1.4
import bb.device 1.4
import bb.system 1.2
import "../components"

Page {
    id: root
    
    property string path: "/"
    property string name: ""
    property string cursor: ""
    property bool hasMore: true
    property int limit: 50
    property variant account: undefined
    property variant spaceUsage: undefined
    
    property int bytesInGB: 1073741824
    property string files_view_key: "files_view"
    property variant files_view_types: {
        GRID: "grid",
        LIST: "list"
    }
    property string default_files_view: files_view_types.LIST 
    
    signal listFolder(string path, string name)
    signal showProps(variant file)
    signal showDownloads()
    signal upload(string path)
    signal shareFolder(string path)
    
    actionBarAutoHideBehavior: ActionBarAutoHideBehavior.HideOnScroll
    actionBarVisibility: ChromeVisibility.Overlay
    
    titleBar: defaultTitleBar
    
    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        
        layout: DockLayout {}
        
        Container {
            verticalAlignment: VerticalAlignment.Center
            
            Header {
                id: header
                visible: path !== ""
                title: path
            }
            
            FilesMover {
                path: root.path
            }
//            
//            FilesUploader {
//                path: root.path
//                
//                onStartUpload: {
//                    uploadsPageRequested();
//                }
//            }

            ListView {
                id: listView
                scrollRole: ScrollRole.Main
                
                property string currentPath: root.path
                
                verticalAlignment: VerticalAlignment.Bottom
                
                margin.leftOffset: ui.du(0.5)
                margin.rightOffset: ui.du(0.5)
                
                dataModel: ArrayDataModel {
                    id: dataModel
                }
                
                layout: {
                    var view = _app.prop(files_view_key, default_files_view);
                    if (view === files_view_types.GRID) {
                        return gridListLayout;
                    }
                    return stackListLayout;
                }
                
                attachedObjects: [
                    ListScrollStateHandler {
                        onScrollingChanged: {
                           if (atEnd && !spinner.running && root.hasMore) {
                              spinner.start();
                              _qdropbox.listFolderContinue(root.cursor);
                           }
                        }
                    }
                ]
                
                function itemType(data, indexPath) {
                    if (layout.objectName === "stackListLayout") {
                        return "listItem";
                    } else {
                        return "gridItem";
                    }
                }
                
                function removeFile(file) {
                    for (var i = 0; i < dataModel.size(); i++) {
                        var data = dataModel.value(i);
                        if (data.id === file.id) {
                            dataModel.removeAt(i);
                            return;
                        }
                    }
                }
                
                function requestPropsPage(file) {
                    root.showProps(file);
                }
                
                function shareFolder(path) {
                    root.shareFolder(path);
                }
                
                function temporaryLinkLoaded(link) {
                    _qdropbox.temporaryLinkLoaded.disconnect(listView.temporaryLinkLoaded);
                    _file.open(link);
                }
                
                onTriggered: {
                    var file = dataModel.data(indexPath);
                    if (file[".tag"] === "folder") {
                        root.listFolder(file.path_display, file.name);
                    } else {
                        _qdropbox.temporaryLinkLoaded.connect(listView.temporaryLinkLoaded);
                        _qdropbox.getTemporaryLink(file.path_display);
                    }
                }
                
                listItemComponents: [
                    ListItemComponent {
                        type: "listItem"
                        StackListItem {
                            tag: ListItemData[".tag"]
                            name: ListItemData.name
                            pathLower: ListItemData.path_lower
                            pathDisplay: ListItemData.path_display
                            fileId: ListItemData.id
                            sharedFolderId: ListItemData.shared_folder_id || ""
                            sharingInfo: ListItemData.sharing_info
                            size: ListItemData.size || 0
                            rev: ListItemData.rev || ""
                            contentHash: ListItemData.content_hash || ""
                            clientModified: ListItemData.client_modified || ""
                            serverModified: ListItemData.server_modified || ""
                            mediaInfo: ListItemData.media_info
                            currentPath: ListItem.view.currentPath
                            thumbnail: ListItemData.thumbnail || ""
                            membersCount: ListItemData.members_count || 0
                            membersCursor: ListItemData.members_cursor || ""
                            isOwner: ListItemData.is_owner || false
                            url: ListItemData.url || ""
                        }
                    },
                    
                    ListItemComponent {
                        type: "gridItem"
                        GridListItem {
                            tag: ListItemData[".tag"]
                            name: ListItemData.name
                            pathLower: ListItemData.path_lower
                            pathDisplay: ListItemData.path_display
                            fileId: ListItemData.id
                            sharedFolderId: ListItemData.shared_folder_id || ""
                            sharingInfo: ListItemData.sharing_info
                            size: ListItemData.size || 0
                            rev: ListItemData.rev || ""
                            contentHash: ListItemData.content_hash || ""
                            clientModified: ListItemData.client_modified || ""
                            serverModified: ListItemData.server_modified || ""
                            mediaInfo: ListItemData.media_info
                            currentPath: ListItem.view.currentPath
                            thumbnail: ListItemData.thumbnail || ""
                            membersCount: ListItemData.members_count || 0
                            membersCursor: ListItemData.members_cursor || ""
                            isOwner: ListItemData.is_owner || false
                            url: ListItemData.url || ""
                        }
                    }
                ]
            }
        }
        
        ActivityIndicator {
            id: spinner
            minWidth: ui.du(20)
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
        }
    }
    
    actions: [
        ActionItem {
            id: createFolder
            title: qsTr("Create folder") + Retranslate.onLocaleOrLanguageChanged
            imageSource: "asset:///images/ic_add_folder.png"
            ActionBar.placement: ActionBarPlacement.Signature
            
            shortcuts: SystemShortcut {
                type: SystemShortcuts.CreateNew
                
                onTriggered: {
                    createFolder.triggered();
                }
            }
            
            onTriggered: {
                root.titleBar = inputTitleBar;
                root.titleBar.focus();
            }
        },
        
        ActionItem {
            id: uploadAction
            title: qsTr("Upload") + Retranslate.onLocaleOrLanguageChanged
            imageSource: "asset:///images/ic_upload.png"
            ActionBar.placement: ActionBarPlacement.OnBar    
            
            onTriggered: {
                upload(root.path);
            }
            
            shortcuts: [
                Shortcut {
                    key: "u"
                    
                    onTriggered: {
                        uploadAction.triggered();
                    }
                }
            ]
        },
        
        ActionItem {
            id: viewActionItem
            imageSource: {
                var view = _app.prop(files_view_key, default_files_view);
                if (view === files_view_types.GRID) {
                    return "asset:///images/ic_view_list.png";
                }
                return "asset:///images/ic_view_grid.png";
            }
            title: {
                var view = _app.prop(files_view_key, default_files_view);
                if (view === files_view_types.GRID) {
                    return qsTr("List") + Retranslate.onLocaleOrLanguageChanged;
                }
                return qsTr("Grid") + Retranslate.onLocaleOrLanguageChanged;
            }
            ActionBar.placement: ActionBarPlacement.OnBar
            
            onTriggered: {
                var view = _app.prop(files_view_key, default_files_view);
                if (view === files_view_types.GRID) {
                    _app.setProp(files_view_key, files_view_types.LIST);
                } else {
                    _app.setProp(files_view_key, files_view_types.GRID);
                }
            }
        },
        
        ActionItem {
            id: downloads
            title: qsTr("Downloads") + Retranslate.onLocaleOrLanguageChanged
            imageSource: "asset:///images/ic_download.png"
            
            onTriggered: {
                root.showDownloads();
            }
        }
    ]
    
    attachedObjects: [
        GridListLayout {
            id: gridListLayout
            objectName: "gridListLayout"
            columnCount: {
                if (rootDisplayInfo.pixelSize.width === 1440) {
                    return 4;
                }
                return 3;
            }
        },
        
        StackListLayout {
            id: stackListLayout
            objectName: "stackListLayout"
        },
        
        DisplayInfo {
            id: rootDisplayInfo
        },
        
        InputTitleBar {
            id: inputTitleBar    
            
            onCancel: {
                inputTitleBar.reset();
                root.titleBar = defaultTitleBar;
            }
            
            onSubmit: {
                if (text !== "") {
                    spinner.start();
                    var fullPath = (root.path === "" ? "/" : root.path) + text;
                    _qdropbox.createFolder(fullPath);
                    root.titleBar.reset();
                    root.titleBar = defaultTitleBar;
                }
            }
        },
        
        TitleBar {
            id: defaultTitleBar
            
            kind: TitleBarKind.FreeForm
            kindProperties: FreeFormTitleBarKindProperties {
                
                content: Container {
                    horizontalAlignment: HorizontalAlignment.Fill
                    verticalAlignment: VerticalAlignment.Fill
                    
                    leftPadding: ui.du(1)
                    topPadding: ui.du(0.5)
                    rightPadding: ui.du(1)
                    bottomPadding: ui.du(1)
                    
                    layout: DockLayout {}
                    
                    Label {
                        visible: root.path !== "/"
                        verticalAlignment: VerticalAlignment.Center
                        text: root.name
                        textStyle.base: SystemDefaults.TextStyles.TitleText
                        textStyle.fontWeight: FontWeight.W500
                    }
                    
                    Label {
                        id: fioLabel
                        visible: root.account !== undefined
                        verticalAlignment: VerticalAlignment.Top
                        textStyle.fontWeight: FontWeight.W500
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 10
                        text: root.account === undefined ? "" : root.account.name.display_name
                    }
                    
                    Label {
                        id: loginLabel
                        visible: root.account !== undefined
                        verticalAlignment: VerticalAlignment.Bottom
                        textStyle.base: SystemDefaults.TextStyles.SubtitleText
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 7
                        text: root.account === undefined ? "" : root.account.email
                    }
                    
                    Label {
                        id: bytesLabel
                        visible: root.spaceUsage !== undefined
                        verticalAlignment: VerticalAlignment.Bottom
                        horizontalAlignment: HorizontalAlignment.Right
                        textStyle.base: SystemDefaults.TextStyles.SubtitleText
                        textStyle.fontSize: FontSize.PointValue
                        textStyle.fontSizeValue: 7
                    }
                }
            }
        }
    ]
    
    onSpaceUsageChanged: {
        if (spaceUsage !== undefined) {
            bytesLabel.text = (Number(spaceUsage.used / root.bytesInGB).toFixed(2) + qsTr("GB") + Retranslate.onLocaleOrLanguageChanged) + 
            "/" + (Number(spaceUsage.allocation.allocated / root.bytesInGB).toFixed(2) + qsTr("GB") + Retranslate.onLocaleOrLanguageChanged);
        }
    }
    
    function append(files) {
        files.forEach(function(f) {
            if (f.shared_folder_id !== "") {
                _qdropbox.listFolderMembers(f.shared_folder_id);
            }
            f.url = _app.getSharedLink(f.path_display).url || ""; 
            dataModel.append(f);
        });
    }
    
    function listFolderLoaded(path, files, cursor, hasMore) {
        spinner.stop();
        if (root.path === path) {
            root.cursor = cursor;
            root.hasMore = hasMore;
            root.append(files);
        }
    }
    
    function listFolderContinueLoaded(files, prevCursor, cursor, hasMore) {
        spinner.stop();
        if (root.cursor === prevCursor) {
            root.cursor = cursor;
            root.hasMore = hasMore;
            root.append(files);
        }
    }
    
    function folderCreated(folder) {
        spinner.stop();
        var p = folder.path_lower;
        if (root.isInRoot(p) || p.indexOf(root.path) !== -1) {
            dataModel.insert(0, folder);
            listView.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.Smooth);
        }
    }
    
    function fileDeleted(file) {
        var p = file.path_display;
        if (root.isInRoot(p) || p.indexOf(root.path) !== -1) {
            for (var i = 0; i < dataModel.size(); i++) {
                if (dataModel.value(i).id === file.id) {
                    dataModel.removeAt(i);
                }
            }
        }
        _qdropbox.getSpaceUsage();
    }
    
    function isDir(file) {
        return file[".tag"] === "folder";
    }
    
    function moved(file) {
        var i = root.isExists(file);
        if (i !== -1) {
            dataModel.removeAt(i);
        } else {
            var p = file.path_display.replace('/' + file.name, '');
            if (p === root.path) {
                if (isDir(file)) {
                    dataModel.insert(0, file);
                    listView.scrollToPosition(ScrollPosition.Beginning, ScrollAnimation.Smooth);
                } else {
                    dataModel.append(file);
                    listView.scrollToPosition(ScrollPosition.End, ScrollAnimation.Smooth);
                }
            }
        }
    }
    
    function renamed(file) {
        var p = file.path_display;
        if (root.isInRoot(p) || p.indexOf(root.path) !== -1) {
            for (var i = 0; i < dataModel.size(); i++) {
                if (dataModel.value(i).id === file.id) {
                    dataModel.replace(i, file);
                }
            }
        }
    }
    
    function isExists(file) {
        for (var i = 0; i < dataModel.size(); i++) {
            if (dataModel.value(i).id === file.id) {
                return i;
            }
        }
        return -1;
    }
    
    function isInRoot(path) {
        return root.path === "" && path.split("/").length === 2;
    }
    
    function thumbnailLoaded(path, localPath) {
        for (var i = 0; i < dataModel.size(); i++) {
            var data = dataModel.value(i);
            if (data.path_display === path) {
                data.thumbnail = localPath;
                dataModel.replace(i, data);
                return;
            }
        }
    }
    
    function reload() {
        dataModel.clear();
        _qdropbox.listFolder(root.path, root.limit);
    }
    
    function refresh() {
        var files = [];
        for (var i = 0; i < dataModel.size(); i++) {
            files.push(dataModel.value(i));
        }
        dataModel.clear();
        dataModel.append(files);
    }
    
    function propChanged(key, val) {
        if (key === files_view_key) {
            if (val === files_view_types.GRID) {
                if (listView.layout !== gridListLayout) {
                    listView.layout = gridListLayout;
                    viewActionItem.imageSource = "asset:///images/ic_view_list.png";
                    viewActionItem.title = qsTr("List") + Retranslate.onLocaleOrLanguageChanged;
                    root.refresh();
                }
            } else {
                if (listView.layout !== stackListLayout) {
                    listView.layout = stackListLayout;
                    viewActionItem.imageSource = "asset:///images/ic_view_grid.png";
                    viewActionItem.title = qsTr("Grid") + Retranslate.onLocaleOrLanguageChanged;
                    root.refresh();
                }
            }
        }
    }
    
    function uploaded(file) {
        var p = file.path_display.replace("/" + file.name, "");
        if (root.path === p) {
            dataModel.append(file);
            listView.scrollToPosition(ScrollPosition.End, ScrollAnimation.Smooth);
        }
        _qdropbox.getSpaceUsage();
    }
    
    function sharedFolder(path, sharedFolderId) {
        if (root.path !== path) {
            for (var i = 0; i < dataModel.size(); i++) {
                var file = dataModel.value(i);
                if (file.path_display === path) {
                    file.shared_folder_id = sharedFolderId;
                    dataModel.replace(i, file);
                    return;
                }
            }
        }
    }
    
    function folderMemberAdded(sharedFolderId) {
        _qdropbox.listFolderMembers(sharedFolderId);
    }
    
    function listFolderMembersLoaded(sharedFolderId, members, cursor) {
        for (var i = 0; i < dataModel.size(); i++) {
            var file = dataModel.value(i);
            if (file.shared_folder_id === sharedFolderId) {
                file.members_count = members.length;
                file.members_cursor = cursor;
                file.is_owner = members.some(function(m) {
                    return m.user.account_id === _app.getAccountId() && m.access_type[".tag"] === "owner";    
                });
                dataModel.replace(i, file);
            }
        }
    }
    
    function unsharedFolder(sharedFolderId) {
        for (var i = 0; i < dataModel.size(); i++) {
            var file = dataModel.value(i);
            if (file.shared_folder_id === sharedFolderId) {
                file.shared_folder_id = "";
                file.members_count = 0;
                file.members_cursor = "";
                dataModel.replace(i, file);
                return;
            }
        }
    }
    
    function sharedLinkCreated(link) {
        for (var i = 0; i < dataModel.size(); i++) {
            var file = dataModel.value(i);
            if (file.path_display === link.path) {
                file.url = link.url;
                dataModel.replace(i, file);
                return;
            }
        }
    }
    
    function folderMemberRemoved(sharedFolderId, member) {
        for (var i = 0; i < dataModel.size(); i++) {
            var file = dataModel.value(i);
            if (file.shared_folder_id && file.shared_folder_id === sharedFolderId) {
                --file.members_count;
                dataModel.replace(i, file);
                return;
            }
        }
    }
    
    function sharedLinkRevoked(url) {
        for (var i = 0; i < dataModel.size(); i++) {
            var file = dataModel.value(i);
            if (file.url === url) {
                file.url = "";
                dataModel.replace(i, file);
                return;
            }
        }
    }
    
    function cleanUp() {
        _qdropbox.popPath();
        _qdropbox.listFolderLoaded.disconnect(root.listFolderLoaded);
        _qdropbox.listFolderContinueLoaded.disconnect(root.listFolderContinueLoaded);
        _qdropbox.folderCreated.disconnect(root.folderCreated);
        _qdropbox.fileDeleted.disconnect(root.fileDeleted);
        _qdropbox.moved.disconnect(root.moved);
        _qdropbox.renamed.disconnect(root.renamed);
        _qdropbox.thumbnailLoaded.disconnect(root.thumbnailLoaded);
        _qdropbox.uploaded.disconnect(root.uploaded);
        _qdropbox.sharedFolder.disconnect(root.sharedFolder);
        _qdropbox.folderMemberAdded.disconnect(root.folderMemberAdded);
        _qdropbox.listFolderMembersLoaded.disconnect(root.listFolderMembersLoaded);
        _qdropbox.unsharedFolder.disconnect(root.unsharedFolder);
        _qdropbox.sharedLinkCreated.disconnect(root.sharedLinkCreated);
        _qdropbox.folderMemberRemoved.disconnect(root.folderMemberRemoved);
        _qdropbox.sharedLinkRevoked.disconnect(root.sharedLinkRevoked);
        _app.propChanged.disconnect(root.propChanged);
    }
    
    onCreationCompleted: {
        _qdropbox.listFolderLoaded.connect(root.listFolderLoaded);
        _qdropbox.listFolderContinueLoaded.connect(root.listFolderContinueLoaded);
        _qdropbox.folderCreated.connect(root.folderCreated);
        _qdropbox.fileDeleted.connect(root.fileDeleted);
        _qdropbox.moved.connect(root.moved);
        _qdropbox.renamed.connect(root.renamed);
        _qdropbox.thumbnailLoaded.connect(root.thumbnailLoaded);
        _qdropbox.uploaded.connect(root.uploaded);
        _qdropbox.sharedFolder.connect(root.sharedFolder);
        _qdropbox.folderMemberAdded.connect(root.folderMemberAdded);
        _qdropbox.listFolderMembersLoaded.connect(root.listFolderMembersLoaded);
        _qdropbox.unsharedFolder.connect(root.unsharedFolder);
        _qdropbox.sharedLinkCreated.connect(root.sharedLinkCreated);
        _qdropbox.folderMemberRemoved.connect(root.folderMemberRemoved);
        _qdropbox.sharedLinkRevoked.connect(root.sharedLinkRevoked);
        _app.propChanged.connect(root.propChanged);
    }
    
    onPathChanged: {
        spinner.start();
        _qdropbox.listFolder(root.path, root.limit);
    }
}
