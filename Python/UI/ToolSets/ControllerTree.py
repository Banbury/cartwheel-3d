'''
Created on 2009-09-26

@author: beaudoin
'''

import UI, wx, PyUtils, os, Core

class ControllerTree(object):
    """A toolset that can be used to display an editable tree of the loaded controllers."""
    
    def __init__(self, toolPanel):
        """Adds a tool set that display a tree of controllers to a toolpanel."""
        self._toolPanel = toolPanel
        toolSet = toolPanel.createToolSet( "Controller Tree", resizable = True )       
        self._toolSet = toolSet
        self._buttonToolbar = toolSet.addTool( ButtonToolbar, 0 )
        self._infoTree = toolSet.addTool( UI.InfoTree, 1, object = wx.GetApp().getControllerList(), desiredHeight = 450 )
        self._infoTree.Bind( wx.EVT_TREE_SEL_CHANGED, self._buttonToolbar.updateButtons )
      
        self._buttonToolbar.setTree( self._infoTree.getTree() )
        
    #
    # Callbacks
    #    
    
    def updateButtons(self, event):
        
        _updateButtonState


class ButtonToolbar(wx.Panel):
    """Private class that provide a toolbar for actions available on a controller tree."""

    def __init__(self,*args, **kwargs):
        super(ButtonToolbar,self).__init__(*args,**kwargs)
    
        self._replaceCurvesButton = wx.BitmapButton( self, bitmap = wx.Bitmap('../data/ui/replaceCurvesButton.png',   wx.BITMAP_TYPE_PNG) )
        self._replaceCurvesButton.SetBitmapDisabled( wx.Bitmap('../data/ui/replaceCurvesButtonDisabled.png',   wx.BITMAP_TYPE_PNG) )
        self._addCurvesButton = wx.BitmapButton( self, bitmap = wx.Bitmap('../data/ui/addCurvesButton.png',   wx.BITMAP_TYPE_PNG) )
        self._addCurvesButton.SetBitmapDisabled( wx.Bitmap('../data/ui/addCurvesButtonDisabled.png',   wx.BITMAP_TYPE_PNG) )
        self._recenterButton = wx.BitmapButton( self, bitmap = wx.Bitmap('../data/ui/recenter.png',   wx.BITMAP_TYPE_PNG) )
        self._recenterButton.SetBitmapDisabled( wx.Bitmap('../data/ui/recenterDisabled.png',   wx.BITMAP_TYPE_PNG) )
        self._stepUntilBreakButton = wx.BitmapButton( self, bitmap = wx.Bitmap('../data/ui/stepUntilBreak.png',   wx.BITMAP_TYPE_PNG) )
        self._stepUntilBreakButton.SetBitmapDisabled( wx.Bitmap('../data/ui/stepUntilBreakDisabled.png',   wx.BITMAP_TYPE_PNG) )
        self._saveButton = wx.BitmapButton( self, bitmap = wx.Bitmap('../data/ui/save.png',   wx.BITMAP_TYPE_PNG) )
        self._saveButton.SetBitmapDisabled( wx.Bitmap('../data/ui/saveDisabled.png',   wx.BITMAP_TYPE_PNG) )
        self._saveCharacterStateButton = wx.BitmapButton( self, bitmap = wx.Bitmap('../data/ui/saveCharacterState.png',   wx.BITMAP_TYPE_PNG) )
        self._saveCharacterStateButton.SetBitmapDisabled( wx.Bitmap('../data/ui/saveCharacterStateDisabled.png',   wx.BITMAP_TYPE_PNG) )
#        self._addButton = wx.BitmapButton( self, bitmap = wx.Bitmap('../data/ui/plusButton.png',   wx.BITMAP_TYPE_PNG) )
#        self._addButton.SetBitmapDisabled( wx.Bitmap('../data/ui/plusButtonDisabled.png',   wx.BITMAP_TYPE_PNG) )
#        self._removeButton = wx.BitmapButton( self, bitmap = wx.Bitmap('../data/ui/minusButton.png',   wx.BITMAP_TYPE_PNG) )
#        self._removeButton.SetBitmapDisabled( wx.Bitmap('../data/ui/minusButtonDisabled.png',   wx.BITMAP_TYPE_PNG) )
#        self._upButton = wx.BitmapButton( self, bitmap = wx.Bitmap('../data/ui/upButton.png',   wx.BITMAP_TYPE_PNG) )
#        self._upButton.SetBitmapDisabled( wx.Bitmap('../data/ui/upButtonDisabled.png',   wx.BITMAP_TYPE_PNG) )
#        self._downButton = wx.BitmapButton( self, bitmap = wx.Bitmap('../data/ui/downButton.png',   wx.BITMAP_TYPE_PNG) )
#        self._downButton.SetBitmapDisabled( wx.Bitmap('../data/ui/downButtonDisabled.png',   wx.BITMAP_TYPE_PNG) )
    
        self._hBoxButtonBar = wx.BoxSizer(wx.HORIZONTAL)
        self._hBoxButtonBar.Add( self._replaceCurvesButton, 0, wx.EXPAND )
        self._hBoxButtonBar.Add( self._addCurvesButton, 0, wx.EXPAND )
        self._hBoxButtonBar.Add( self._recenterButton, 0, wx.EXPAND )
        self._hBoxButtonBar.Add( self._stepUntilBreakButton, 0, wx.EXPAND )
        self._hBoxButtonBar.AddStretchSpacer()
        self._hBoxButtonBar.Add( self._saveButton, 0, wx.EXPAND )
        self._hBoxButtonBar.Add( self._saveCharacterStateButton, 0, wx.EXPAND )
#        self._hBoxButtonBar.Add( self._addButton, 0, wx.EXPAND )
#        self._hBoxButtonBar.Add( self._removeButton, 0, wx.EXPAND )
#        self._hBoxButtonBar.Add( self._upButton, 0, wx.EXPAND )
#        self._hBoxButtonBar.Add( self._downButton, 0, wx.EXPAND )
        self.SetSizer( self._hBoxButtonBar )

        self._replaceCurvesButton.Bind( wx.EVT_BUTTON, self.replaceCurves )
        self._addCurvesButton.Bind( wx.EVT_BUTTON, self.addCurves )
        self._recenterButton.Bind( wx.EVT_BUTTON, self.recenter )
        self._stepUntilBreakButton.Bind( wx.EVT_BUTTON, self.stepUntilBreak )
        self._saveButton.Bind( wx.EVT_BUTTON, self.saveController )
        self._saveCharacterStateButton.Bind( wx.EVT_BUTTON, self.saveCharacterState )

        self._tree = None

        self._saveNumber = 0 
        self._dirName = os.path.join( 'Data', 'Temp' )

        self._lastSave = (None,None,None)  # a triplet: controller, saveNumberForController, saveNumberForCharacterState



    #
    # Public methods
    #

    def setTree(self, tree):
        """Sets the tree attached to this button toolbar."""
        self._tree = tree
        self.updateButtons()

    #
    # Callbacks
    #

    def updateButtons(self, event = None):
        """Adjust button states based on the current selection"""
        treeItemId = self._tree.GetSelection()
        operations = save = plus = minus = up = down = False
        if treeItemId.IsOk():
            element = self._tree.GetItemPyData( treeItemId )
            operations = save = True
        self._replaceCurvesButton.Enable(operations)
        self._addCurvesButton.Enable(operations)
        self._recenterButton.Enable(operations)
        self._stepUntilBreakButton.Enable(operations)
        self._saveButton.Enable(save)
        self._saveCharacterStateButton.Enable(save)
#        self._addButton.Enable(plus)
#        self._removeButton.Enable(minus)
#        self._upButton.Enable(up)
#        self._downButton.Enable(down)

    def replaceCurves(self, event = None):
        """Replace all the curves with the selected ones."""
        app = wx.GetApp()
        app.clearCurves()
        self.addCurves( event )

    def addCurves(self, event = None):
        """Add all the curves under a node to the application."""
        app = wx.GetApp()

        treeItemId = self._tree.GetSelection()
        if not treeItemId.IsOk(): return
        
        # Name that node by going back to the top of the tree
        nameStack = []
        currTreeItemId = treeItemId
        rootTreeItemId = self._tree.GetRootItem()
        while currTreeItemId.IsOk() and currTreeItemId != rootTreeItemId:
            nameStack.insert(0, self._tree.GetItemText(currTreeItemId))
            currTreeItemId = self._tree.GetItemParent(currTreeItemId)
        
        # Now, recursively go down the tree from the current node
        curveList = app.getCurveList()
        curveList.beginBatchChanges()
        try: self._addCurves( treeItemId, nameStack )
        finally: curveList.endBatchChanges()
        

    def saveController(self, event = None):
        """Save the currently selected controller"""
        controller = self._getSelectedController()
        if controller is None : return
        
        saveNumber = self._saveNumber
        if PyUtils.sameObject( self._lastSave[0], controller ) :
            if self._lastSave[2] is not None :
                saveNumber = self._lastSave[2]

        controllerName = controller.getName()
        dialogTitle = "Save %s Controller" % controllerName
        fileName = "%s_%d.py" % (controllerName, saveNumber)
        
        dialog = wx.FileDialog(self, dialogTitle, self._dirName, fileName, "*.py", wx.SAVE | wx.OVERWRITE_PROMPT )
        dialog.CenterOnScreen()
        if dialog.ShowModal() == wx.ID_OK:
            if saveNumber != self._saveNumber:
                self._lastSave = (None, None, None)
            else:
                self._lastSave = (controller, self._saveNumber, None)
                self._saveNumber += 1
            fileName=dialog.GetFilename()
            self._dirName=dialog.GetDirectory()
            pathName = os.path.join(self._dirName,fileName)
            file = open(pathName,'w')
            file.write( "from App.Proxys import *\n\ndata = %s" % PyUtils.fancify( PyUtils.serialize(controller)) )
            file.close()
        dialog.Destroy()
            
    def recenter(self, event = None):
        """Recenter the character attached to the currently selected controller"""
        
        controller = self._getSelectedController()
        if controller is None : return
        app = wx.GetApp()
        app.recenterCharacter( controller.getCharacter() )

    def stepUntilBreak(self, event = None):
        """Steps the selected controller until its phase resets to zero"""
        
        controller = self._getSelectedController()
        if controller is None : return
        app = wx.GetApp()
        app.advanceAnimationUntilControllerEnds( controller )

    def saveCharacterState(self, event = None):
        """Saves the selected character state"""
        
        controller = self._getSelectedController()
        if controller is None : return
        app = wx.GetApp()
        character = controller.getCharacter()
        
        saveNumber = self._saveNumber 
        if PyUtils.sameObject( self._lastSave[0], controller ) :
            if self._lastSave[1] is not None :
                saveNumber = self._lastSave[1]

        controllerName = controller.getName()
        dialogTitle = "Save Character State for %s" % controllerName
        fileName = "%sState_%d.rs" % (controllerName, saveNumber)
        
        dialog = wx.FileDialog(self, dialogTitle, self._dirName, fileName, "*.rs", wx.SAVE | wx.OVERWRITE_PROMPT )
        dialog.CenterOnScreen()
        if dialog.ShowModal() == wx.ID_OK:
            if saveNumber != self._saveNumber:
                self._lastSave = (None, None, None)
            else:
                self._lastSave = (controller, None, self._saveNumber)
                self._saveNumber += 1
            fileName=dialog.GetFilename()
            self._dirName=dialog.GetDirectory()
            pathName = os.path.join(self._dirName,fileName)
            stateArray = Core.ReducedCharacterStateArray()
            if controller.getStance() == Core.RIGHT_STANCE:
                character.getReverseStanceState(stateArray)
            else:  
                character.getState(stateArray)
            character.saveReducedStateToFile( str(pathName), stateArray )
        dialog.Destroy()
        
    #
    # Private methods
    #
    

    def _getSelectedController(self):
        """Go up to the root to find the selected controller. None if no controller was selected."""        
        treeItemId = self._tree.GetSelection()
        rootTreeItemId = self._tree.GetRootItem()
        controller = None
        while treeItemId.IsOk() and treeItemId != rootTreeItemId:
            try: controller = self._tree.GetItemPyData(treeItemId).getObject()
            except AttributeError: controller = None
            treeItemId = self._tree.GetItemParent(treeItemId)
        if controller is None or not isinstance(controller,Core.SimBiController): 
            return None
        return controller
    
    
    def _addCurves(self, treeItemId, nameStack):
        """PRIVATE. Recursively add curves under this treeItemId."""
        from App.Proxys import Member
        app = wx.GetApp()

        controller = self._getSelectedController()
        phiPtr = controller.getPhiPtr()

        nodeData = self._tree.GetItemPyData( treeItemId )
        if nodeData is None : return
        try:
            # Assume the tree node contains an object
            object = nodeData.getObject()            
        except AttributeError:
            object = None
                
        if object is not None:
            proxyObject = PyUtils.wrap(object, recursive = False)
            for i in range( proxyObject.getMemberCount() ):
                value, member =  proxyObject.getValueAndInfo(i)
                if value is None: continue
                if isinstance( member, Member.Trajectory1d ):
                    name = " : ".join(nameStack[1:])
                    if member.name != "baseTrajectory" :
                        name += " : " + member.fancyName
                    app.addCurve( name, value, phiPtr )

        # Recurse on the child nodes
        currTreeItemId, cookie = self._tree.GetFirstChild( treeItemId )
        while currTreeItemId.IsOk():
            newNameStack = nameStack[:]
            newNameStack.append( self._tree.GetItemText(currTreeItemId)  )
            self._addCurves( currTreeItemId, newNameStack )
            currTreeItemId, cookie = self._tree.GetNextChild( treeItemId, cookie )        
    
