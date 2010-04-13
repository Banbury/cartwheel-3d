'''
Created on 2009-08-30

@author: beaudoin

Allows creation of a vertical collection of collapsible tool sets
'''

import wx

class ToolSet(wx.Panel):
    """A simple class that maintain tools and that can be hidden or showed."""
    _triUp   = None
    _triDown = None        
    
    def __init__(self, parent, id = wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.TAB_TRAVERSAL,
                 name='', resizable = False ):
        
        super(ToolSet, self).__init__(parent, id, pos, size, style, name)
        
        # Load the bitmaps if needed
        if ToolSet._triUp is None :
            ToolSet._triUp   = wx.Bitmap('../data/ui/triUp.png',   wx.BITMAP_TYPE_PNG)
            ToolSet._triDown = wx.Bitmap('../data/ui/triDown.png', wx.BITMAP_TYPE_PNG)
        
        # Setup a link to the parent, to refresh it when tool sets open or close
        self._parentVBox = parent._vBox
        
        # Setup the tool set title
        self._title = wx.Panel(self)
        self._title.SetBackgroundColour( (180,180,180) )

        titleLabel = wx.StaticText(self._title,label=name)        
        self._titleBitmap = wx.StaticBitmap(self._title, bitmap = ToolSet._triUp )
        
        titleHBox = wx.BoxSizer(wx.HORIZONTAL)
        titleHBox.Add( self._titleBitmap, 0, wx.CENTER | wx.ALL, 3 )
        titleHBox.Add( titleLabel, 1, wx.ALL, 3 )
        self._title.SetSizer( titleHBox )
            
        # A 1 px panel for spacing
        spacing = wx.Panel(self, size=(-1,1) ) 
        spacing.SetBackgroundColour( (150,150,150) )

        self._vBox = wx.BoxSizer(wx.VERTICAL)
        self._vBox.Add( self._title, 0, wx.EXPAND )        
        self._vBox.Add( spacing, 0, wx.EXPAND )        

        # Create the panel
        # If the tool set is resizable, create the panel in a sash window
        if resizable:
            # Setup the main tool set panel
            self._sashWindow = wx.SashWindow(self)
            self._sashWindow.SetSashVisible( wx.SASH_BOTTOM, True )
            self._vBox.Add( self._sashWindow, 0, wx.EXPAND )

            self._panel = wx.Panel(self._sashWindow)
            self._vBoxSashWindow = wx.BoxSizer(wx.VERTICAL)
            self._vBoxSashWindow.Add( self._panel, 0, wx.EXPAND )
            self._sashWindow.SetSizer( self._vBoxSashWindow )
            
            self._sashWindow.Bind( wx.EVT_SASH_DRAGGED, self.toolSetResize )
        else:        
            # Setup the main tool set panel
            self._panel = wx.Panel(self)
            self._vBox.Add( self._panel, 0, wx.EXPAND )
        
        # Create the sizer for the panel
        self._panelVBox = wx.BoxSizer(wx.VERTICAL)
        self._panel.SetSizer( self._panelVBox )                

        self.SetSizer( self._vBox )
        
        # Setup initial state
        self._opened = True        

        # Bind events
        self._title.Bind( wx.EVT_LEFT_DCLICK, self.processOpenClose )       
        titleLabel.Bind( wx.EVT_LEFT_DCLICK, self.processOpenClose )       
        self._titleBitmap.Bind( wx.EVT_LEFT_DOWN, self.processOpenClose )       
        self._titleBitmap.Bind( wx.EVT_LEFT_DCLICK, self.processOpenClose )       

    #
    # Event handlers

    def processOpenClose( self, event ):
        """Change the state from open to close and vice-versa"""
        self.setOpen( not self._opened )
        
    def toolSetResize( self, event ):
        """Change the size of the tool set"""
        if event.GetDragStatus() == wx.SASH_STATUS_OUT_OF_RANGE :
            return

        self._panelVBox.SetMinSize( (-1, event.GetDragRect().height) )
        self.refreshAll()
    
    #
    # public methods

    def setOpen( self, opened ):
        """Indicates whether this tool set should be open or not"""
        if self._opened == opened :
            return
        
        self._opened = opened;
        try:
            self._sashWindow.Show( opened )
        except AttributeError:
            self._panel.Show( opened )
        if opened :
            self._titleBitmap.SetBitmap( ToolSet._triUp )
        else :
            self._titleBitmap.SetBitmap( ToolSet._triDown )
        self._parentVBox.Layout()
        self.GetParent().FitInside()
        self.GetParent().Refresh()

    def addTool(self, widgetClass, proportion=0, flag=wx.EXPAND, border=0, desiredHeight = 0, **argkw ):
        """Adds the specified widget as a tool in the tool set
        widgetCreator is a standard wxWidget class, such as wx.panel,
        you can pass any wxWidget creation argument you want in argkw.
        The method returns the newly created widget."""
        widget = widgetClass( self._panel, **argkw )
        self._panelVBox.Add( widget, proportion, flag, border )
        minSize = self._panelVBox.GetMinSizeTuple()
        self._panelVBox.SetMinSize((minSize[0],desiredHeight))
        self.refreshAll()
        return widget

    def refreshAll(self):
        """Refresh the content of the tool set"""
        self._panel.Fit()
        try: 
            self._sashWindow.Fit()
        except AttributeError: pass
        self.Fit()
        self.GetParent().Layout()
        
        
class ToolPanel(wx.ScrolledWindow):
    """A simple class that maintain tool sets and that can be scrolled."""
           
    def __init__(self, parent, id = wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style= wx.HSCROLL | wx.VSCROLL,
                 name='toolPanel' ):
        
        super(ToolPanel, self).__init__(parent, id, pos, size, style, name)

        self._vBox = wx.BoxSizer(wx.VERTICAL)
        self.SetSizer( self._vBox )
        
        # Enable vertical scrolling by 10 pixel increments
        self.SetScrollRate(0,10)


    def createToolSet(self, toolSetName, resizable = False):
        """Creates a new tool set and adds it to the ToolPanel.
        The user should then add tools to the tool set in any way he wishes
        """
        toolSet = ToolSet(self, name=toolSetName, resizable = resizable )
        self._vBox.Add( toolSet, 0, wx.EXPAND )
        self.Layout()
        return toolSet
        