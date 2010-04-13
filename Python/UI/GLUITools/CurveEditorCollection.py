'''
Created on 2009-09-30

@author: beaudoin
'''


import wx, GLUtils, PyUtils

class CurveEditorCollection(GLUtils.GLUIContainer):
    """A collection of curve editor that observes the curve editors of the application."""
    
    def __init__( self, parent, x=0, y=0, width=0, height=0, minWidth=-1, minHeight=-1 ):
        
        super(CurveEditorCollection,self).__init__(parent,x,y,width,height,minWidth,minHeight)
        
        sizer = GLUtils.GLUIBoxSizer( GLUtils.GLUI_HORIZONTAL )
        self.setSizer(sizer)
        
        app = wx.GetApp()
        self._appCurveList = app.getCurveList()
        self._appCurveList.addObserver(self)
        
        self._curveEditorList = []
        
    def _removeCurveEditors(self, start=None, end=None):
        """Private. This method removes curve editors from the curve editor list.
        Objects removed are in the range [start,end).
        Runs from object 0 when start==None and until the end when end==None."""
        if start is None : start = 0
        if end is None : end = len( self._curveEditorList )
        for curveEditor in self._curveEditorList[start:end]:
            self.detachChild( curveEditor )
        del self._curveEditorList[start:end]
        
    def _insertCurveEditor(self, index, curve):
        """Private. This method inserts at the specified index a curve editor that wraps the specified App.Curve."""
        curveEditor = GLUtils.GLUICurveEditor( self )
        curveEditor.setTrajectory( curve.getTrajectory1d() )
        curveEditor.setCurrTime( curve.getPhiPtr() )
        curveEditor.setTitle( curve.getName() )
        curveEditor.setMinSize(200,200)
        self.getSizer().add( curveEditor )
        self._curveEditorList.insert(index, curveEditor)
        
    def getTrajectory(self, index):
        """Return the trajectory attached to the curve editor number 'index'."""
        return self._curveEditorList[index].getTrajectory()
        
        
    def update(self, data = None):
        """Called whenever the curve list is updated. Make sure the curve editor list match the application curve list."""
        
        count = self._appCurveList.getCount()
        for i in range(count):
            inCurve = self._appCurveList.get(i)
            inTrajectory1d = inCurve.getTrajectory1d()
            try: 
                outTrajectory1d = self.getTrajectory(i)
                areSameObject = PyUtils.sameObject(inTrajectory1d, outTrajectory1d)
            except IndexError: 
                outTrajectory1d = None
                areSameObject = False        
            if not areSameObject:
                # Need to delete or insert
                # First, check how many curves we should delete
                delete = 1 
                try: 
                    while not PyUtils.sameObject( inTrajectory1d, self.getTrajectory(i+delete) ) :
                        delete += 1
                except IndexError:
                    delete = 0
            
                if delete > 0 :
                    # Delete the specified controllers
                    self._removeCurveEditors(i,i+delete)
                else :
                    # Insert the specified controller                                                        
                    self._insertCurveEditor(i, inCurve)
        
        # Delete any remaining controllers
        self._removeCurveEditors(count)    
        self.getParent().layout()
