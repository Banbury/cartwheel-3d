'''
Created on 2009-10-08

@author: beaudoin
'''

import wx

class ToggleBitmapButton(wx.BitmapButton):
    
    def __init__(self, parent, id=wx.ID_ANY, bitmap=wx.NullBitmap, pos=wx.DefaultPosition, size=wx.DefaultSize, style=wx.BU_AUTODRAW, validator=wx.DefaultValidator, name=wx.ButtonNameStr, selected=False):
        """Creates a bitmap button that can be toggled on and off."""
        
        super(ToggleBitmapButton,self).__init__( parent, id, bitmap, pos, size, style, validator, name)
        
        self._selected = selected
        self._lookIsSelected = False

        self.Bind( wx.EVT_BUTTON, self.OnClick )

    #
    # Callbacks
    #

    def OnClick(self, event):
        """Flips the state if possible."""
        self._selected = not self._selected
        self._updateLook()
        event.Skip()

    #
    # Public methods
    #

    def SetBitmapSelected(self, bitmap):
        """Sets the bitmap for the selected (depressed) button appearance."""
        super(ToggleBitmapButton,self).SetBitmapSelected(bitmap)
        self._updateLook()
        
    def SetSelected(self, selected = True):
        """Indicates whether this toggle button should be selected or not."""
        self._selected = selected
        self._updateLook()
        
    def IsSelected(self):
        """Returns true if the toggle button is currently in its selected state, false otherwise."""
        return self._selected
        
    #
    # Private methods
    #
    
    def _updateLook(self):
        """Make sure the look matches the selected state."""
        if self._selected == self._lookIsSelected : return
        
        # Toggle looks
        other = self.GetBitmapSelected()
        if not other.IsOk(): return
        current = self.GetBitmapLabel()
        self.SetBitmapLabel( other )
        super(ToggleBitmapButton,self).SetBitmapSelected( current )
        self._lookIsSelected = self._selected
        
        
        
    