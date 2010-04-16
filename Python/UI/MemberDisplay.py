'''
Created on 2009-09-24

@author: beaudoin
'''

import Utils, wx
import PyUtils 


class MemberDisplay(Utils.Observer):
    """
    This class wraps an element and displays its content in a table with a grid sizer
    """
    
    def __init__(self, table, sizer):
        super(MemberDisplay,self).__init__()
        self._table = table
        self._sizer = sizer
        self._contentControls = None
        self._object = None

    def __del__(self):
        try: self.deleteObserver()
        except AttributeError: pass
        
    def deleteObserver(self):
        try: self._object.deleteObserver(self)        
        except AttributeError: pass
        
    def attachObject(self, object):
        self.deleteObserver()
        self._object = object
        try: 
            self._proxy = PyUtils.wrap( object, recursive = False )
        except AttributeError:
            self._proxy = None            
        try: object.addObserver(self)
        except AttributeError: pass
        self.createControls()
        
    def createControls(self):
        """Creates all the controls to display the attached object's content"""
        self._table.Freeze()
        self._table.DestroyChildren()
        self._contentControls = []
        if self._proxy is None :
             self._table.SetVirtualSize((0,0))
             self._table.Thaw()
             return
        for i in range( self._proxy.getMemberCount() ) :
            ( value, info ) = self._proxy.getValueAndInfo(i)
            if info.isObject : continue
            value = info.format(value)
            labelControl = wx.StaticText(self._table, label=info.fancyName)
            if info.editable :
                contentControl = self.createControlForMember( self._table, info, value)
            else :
                contentControl = wx.StaticText(self._table, label=str(value))
            self._sizer.Add( labelControl, 0, wx.ALIGN_CENTER_VERTICAL | wx.LEFT | wx.TOP , 2)
            self._sizer.Add( contentControl, 0, wx.EXPAND | wx.TOP | wx.RIGHT, 2)
            self._contentControls.append( contentControl )
        
        self._table.FitInside()
        self._table.Thaw()
        
    def createControlForMember( self, parent, info, value ):
        """Creates a control for the member specified in info."""
        from App.Proxys import Member
        if isinstance(info, Member.Enum) :
            result = wx.ComboBox( parent, value=value, style = wx.CB_READONLY, choices = info.enum.choices() )
            result.Bind( wx.EVT_COMBOBOX, lambda event: self.changeValue(info.name, result.GetValue()) )
        elif info.type == bool :
            result = wx.CheckBox( parent )
            result.SetValue(value)
            result.Bind( wx.EVT_CHECKBOX, lambda event: self.changeValue(info.name, result.GetValue()) )
        else:
            result = wx.TextCtrl(parent, value=str(value))
            result.Bind( wx.EVT_KILL_FOCUS, lambda event: self.changeValue(info.name, result.GetValue()) )
        return result

    def changeValue(self, memberName, value):
        """The value of the control specified in info has changed."""
        try: self._proxy.setValueAndUpdateObject( memberName, value, self._object )
        except: self.update()

    def update(self, data = None):
        """Called whenever the observer is updated."""
        self._proxy.extractFromObject( self._object, recursive = False )
        j = 0
        for i in range( self._proxy.getMemberCount() ) :
            ( value, info ) = self._proxy.getValueAndInfo(i)
            if info.isObject : continue
            value = info.format(value)
            if not info.type is bool : value = str(value)
            control = self._contentControls[j]
            try: self._contentControls[j].SetValue(value)                   # Try to set the value
            except AttributeError: self._contentControls[j].SetLabel(value) # Otherwise SetLabel will do
            j += 1    
    