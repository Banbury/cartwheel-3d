'''
Created on 2009-10-02

@author: beaudoin
'''

import wx, UI, PyUtils

class SnapshotTree(object):
    
    def __init__(self, toolPanel):
        """Adds a tool set for animation information to a toolpanel."""
        self._toolPanel = toolPanel
        self._toolSet = toolPanel.createToolSet( "Snapshots", resizable = True )       
        app = wx.GetApp()

        buttonPanel  = self._toolSet.addTool( wx.Panel )
        self._takeSnapshotButton = wx.BitmapButton( buttonPanel, bitmap = wx.Bitmap('../data/ui/takeSnapshotButton.png',   wx.BITMAP_TYPE_PNG) )        
        self._takeSnapshotButton.SetBitmapDisabled( wx.Bitmap('../data/ui/takeSnapshotButtonDisabled.png',   wx.BITMAP_TYPE_PNG) )
        self._dontRestoreControllerParamsButton = UI.Ext.ToggleBitmapButton( buttonPanel, bitmap = wx.Bitmap('../data/ui/restoreControllerParams.png',   wx.BITMAP_TYPE_PNG) )
        self._dontRestoreControllerParamsButton.SetBitmapSelected( wx.Bitmap('../data/ui/dontRestoreControllerParams.png',   wx.BITMAP_TYPE_PNG) )
        self._previousButton = wx.BitmapButton( buttonPanel, bitmap = wx.Bitmap('../data/ui/previousSnapshotButton.png',   wx.BITMAP_TYPE_PNG) )
        self._previousButton.SetBitmapDisabled( wx.Bitmap('../data/ui/previousSnapshotButtonDisabled.png',   wx.BITMAP_TYPE_PNG) )
        self._restoreButton = wx.BitmapButton( buttonPanel, bitmap = wx.Bitmap('../data/ui/restoreSnapshotButton.png',   wx.BITMAP_TYPE_PNG) )
        self._restoreButton.SetBitmapDisabled( wx.Bitmap('../data/ui/restoreSnapshotButtonDisabled.png',   wx.BITMAP_TYPE_PNG) )
        self._nextButton = wx.BitmapButton( buttonPanel, bitmap = wx.Bitmap('../data/ui/nextSnapshotButton.png',   wx.BITMAP_TYPE_PNG) )
        self._nextButton.SetBitmapDisabled( wx.Bitmap('../data/ui/nextSnapshotButtonDisabled.png',   wx.BITMAP_TYPE_PNG) )
        
        self._takeSnapshotButton.Bind( wx.EVT_BUTTON, lambda e: app.takeSnapshot() )
        self._previousButton.Bind( wx.EVT_BUTTON, lambda e: app.previousSnapshot(self.restoreControllerParams()) )
        self._restoreButton.Bind( wx.EVT_BUTTON, lambda e: app.restoreActiveSnapshot(self.restoreControllerParams()) )
        self._nextButton.Bind( wx.EVT_BUTTON, lambda e: app.nextSnapshot(self.restoreControllerParams()) )
        
        self._hBoxButtons = wx.BoxSizer( wx.HORIZONTAL )
        self._hBoxButtons.Add(self._takeSnapshotButton)
        self._hBoxButtons.AddStretchSpacer(1)
        self._hBoxButtons.Add(self._dontRestoreControllerParamsButton)
        self._hBoxButtons.Add(self._previousButton)
        self._hBoxButtons.Add(self._restoreButton)
        self._hBoxButtons.Add(self._nextButton)
        
        buttonPanel.SetSizerAndFit(self._hBoxButtons)

        self._infoTree = self._toolSet.addTool( UI.InfoTreeBasic, 1, object = app.getSnapshotTree(), desiredHeight = 200, autoVisible = True, onUpdate = self.update )
        self._infoTree.Bind( wx.EVT_TREE_ITEM_ACTIVATED , self.selectSnapshot )

        self._activeTreeItemId = None

    #
    # Public methods
    #
    
    def restoreControllerParams(self):
        """Return true if we should restore the controller parameters when moving around, false if not."""
        return not self._dontRestoreControllerParamsButton.IsSelected()

    #
    # Callbacks    
    #

    def selectSnapshot(self, event):
        """Select the snapshot double-clicked at."""
        item = event.GetItem()
        try:
            nodeData = self._infoTree.GetItemPyData(item)
            snapshot = nodeData.getObject()
            snapshot.restore(self.restoreControllerParams())
        except AttributeError:
            event.Skip()
        
    def update(self, data = None):
        """Called whenever the tree is updated."""
        try: 
            tree = self._infoTree
            rootItem = tree.GetRootItem()
            nodeData = tree.GetItemPyData( rootItem )
            snapshot = nodeData.getObject().getCurrentSnapshot()
        except AttributeError: return
        
        if self._activeTreeItemId != None :
            currentSnapshot = tree.GetItemPyData( self._activeTreeItemId )
            if PyUtils.sameObject(currentSnapshot, snapshot) : return
            tree.SetItemBold( self._activeTreeItemId, False )
        
        # Look for the new item
        activeList = [tree.GetFirstChild(rootItem)[0]]
        while len(activeList) > 0 :
            treeItemId = activeList.pop()
            if not treeItemId.IsOk(): continue
            object = tree.GetItemPyData( treeItemId ).getObject()
            if PyUtils.sameObject(snapshot, object) :
                self._activeTreeItemId = treeItemId
                tree.SetItemBold( treeItemId, True )
                return
            activeList.append( tree.GetFirstChild(treeItemId)[0] )
            activeList.append( tree.GetNextSibling(treeItemId) )
        
