'''
Created on 2009-09-14

@author: beaudoin
'''

import wx
import Utils
from MemberDisplay import MemberDisplay

# Unique list for all the images
_iconList = None 
_iconDict = {}
_unknownIcon = '../data/ui/unknownIcon.png'

def _getIconList():
    global _iconList
    if _iconList is None :
        _iconList = wx.ImageList(16,16, False)
    return _iconList

def _addIcon( pngFilename ):
    """Private, adds an icon to the list of icon and the dictionnary."""
    global _iconDict, _unknownIcon
    iconList = _getIconList()
    if pngFilename == None: pngFilename = _unknownIcon
    bitmap = wx.Bitmap(pngFilename, wx.BITMAP_TYPE_PNG)
    if not bitmap.Ok() :
        if pngFilename == _unknownIcon :
            raise IOError("Icon file '%s' not found" % _unknownIcon)
        return _addIcon(None)        
    index = iconList.Add(bitmap)
    _iconDict[pngFilename] = index
    return index

def getIconIndex( pngFilename ):
    """Returns the index of the icon for the pngFilename. 
    Use this to set node images in the info tree."""
    global _iconDict
    try: return _iconDict[pngFilename]
    except KeyError: return _addIcon( pngFilename ) 


class InfoTreeBasic(wx.TreeCtrl):
    """A class that display a tree structure containing all the information of a given wrappable and observable object."""

    def __init__(self, parent, object = None, rootName = "Root", autoVisible = False, onUpdate = None, id = wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style= wx.TR_HAS_BUTTONS | wx.TR_HIDE_ROOT | wx.TR_LINES_AT_ROOT,
                 validator = wx.DefaultValidator, name='InfoTreeBasic'):
        """Initializes a tree that listens to the object."""
        
        super(InfoTreeBasic, self).__init__(parent, id, pos, size, style, validator, name)
        
        # Configure empty tree
        self.SetImageList( _getIconList() )
        self.AddRoot( "" )

        # Set a reasonable minimum width
        self.SetMinSize( (-1,80) )

        self._rootName = rootName

        self._autoVisible = autoVisible
        
        self._onUpdate = onUpdate

        self._updateLocks = 0


        # Create an observer that need to be alive for as long as the tree is alive
        # so store it in a member
        self.attachToObject( object )
        
    def isAutoVisible(self):
        """True if the tree should make new nodes visible when they are added."""
        return self._autoVisible

    def attachToObject(self, object):
        """Attach this tree to a specific object."""
        from App.Proxys.NodeData import NodeData 

        if object is not None:
            self._root = NodeData( self._rootName, object, self, self.GetRootItem() )
            


    #
    # Callbacks
    #
    def updateLock(self):
        """Called whenever something in the tree is updated. Call that to set a lock. The update
        will take place once all the locks are released."""
        self._updateLocks += 1

    def updateUnlock(self):
        """Called whenever something in the tree is updated. Call that to release a lock. The update
        will take place once all the locks are released."""
        assert self._updateLocks > 0, "Update unlocked, but no lock set!"
        self._updateLocks -= 1
        if self._updateLocks == 0 : 
            self.update()


    def update(self, data = None):
        """Called whenever something in the tree is updated."""
        try: self._onUpdate(data)
        except TypeError: pass


class InfoTree(wx.Panel):
    """A complete tree attached to an info box.""" 

    def __init__(self, parent, object = None, rootName = "Root", autoVisible = False, onUpdate = None, id = wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style= wx.TAB_TRAVERSAL, name='InfoTree'):

        super(InfoTree, self).__init__(parent, id, pos, size, style, name)

        self._infoTreeBasic = InfoTreeBasic( self, object, rootName, autoVisible, onUpdate )
        
        self._sashWindow = wx.SashWindow( self )      
        self._sashWindow.SetSashVisible( wx.SASH_TOP, True )

        self._infoTable = wx.ScrolledWindow(self._sashWindow, style = wx.VSCROLL )
        self._infoTable.SetBackgroundColour( (255,255,255) )
        self._infoTable.SetScrollRate(0,10)
        self._infoTable.SetMinSize( (-1,70) )

        self._infoTableSizer = wx.FlexGridSizer(0,2,0,8)
        self._infoTableSizer.AddGrowableCol(1)
        self._infoTable.SetSizer( self._infoTableSizer )
        
        self._vBoxSashWindow = wx.BoxSizer(wx.VERTICAL)
        self._vBoxSashWindow.Add( self._infoTable, 1, wx.EXPAND )
        self._vBoxSashWindow.SetMinSize( (-1,100) )
        self._sashWindow.SetSizer( self._vBoxSashWindow )
        self._sashWindow.Bind( wx.EVT_SASH_DRAGGED, self.sashWindowResize )

        self._vBox = wx.BoxSizer(wx.VERTICAL)
        self._vBox.Add( self._infoTreeBasic, 1, wx.EXPAND )
        self._vBox.Add( self._sashWindow, 0, wx.EXPAND )
        self.SetSizerAndFit( self._vBox )        
            
        self._memberDisplay = MemberDisplay(self._infoTable, self._infoTableSizer)
        
        self._infoTreeBasic.Bind( wx.EVT_TREE_SEL_CHANGED, self.selectionChanged )
        
        self.Bind( wx.EVT_PAINT, self.onPaint )
    #
    # Private methods
    #
    def _adjustSashSize(self):
        """Private. Make sure sash size is not too large."""
        height = self._vBoxSashWindow.GetMinSize()[1] 
        maxHeight = self.GetSize()[1] - 80
        if height > maxHeight :
            if maxHeight <= 0 : maxHeight = -1
            currMaxHeight = self._vBoxSashWindow.GetMinSize().height
            if currMaxHeight == maxHeight : return
            self._vBoxSashWindow.SetMinSize( (-1,maxHeight) )
            self._refreshAll()

    def _refreshAll(self):
        """Private. Make sure everything is laid-out correctly."""
        self.Fit()
        self.GetParent().Layout()
            
    def _displayInfo(self, object):
        """Display info on the passed object."""
        self._memberDisplay.attachObject(object)        
    #
    # Public methods    
    #
    
    def attachToObject(self, object):
        self._infoTreeBasic.attachToObject(object)

    def getTree(self):
        return self._infoTreeBasic
    
    #
    # Callbacks
    #
        
    def onPaint(self, event):
        self._adjustSashSize()
        event.Skip()
        
    def sashWindowResize(self, event):
        """Called whenever the sash window is resized"""
        if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE :
            return
        self._vBoxSashWindow.SetMinSize( (-1,event.GetDragRect().height) )
        self._adjustSashSize()
        self._refreshAll()

    def selectionChanged(self, event):
        nodeData = self._infoTreeBasic.GetItemPyData(event.GetItem())
        try: object = nodeData.getObject()
        except (AttributeError, TypeError): object = nodeData  
        self._displayInfo( object )
        event.Skip()
        
        

