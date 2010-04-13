'''
Created on 2009-09-26

@author: beaudoin
'''

import UI, wx, math

class _OptionData(object):
    
    def __init__(self, checkBox, getter, setter):
        self.checkBox = checkBox
        self.getter = getter
        self.setter = setter
        self.update()
       
    def update(self):
        self.checkBox.SetValue( self.getter() )
       
    def set(self):
        self.setter(self.checkBox.IsChecked())
       
class _SliderData(object):
    
    def __init__(self, slider, valueWidget, getter, setter, min, max):
        self.slider = slider
        self.valueWidget = valueWidget
        self.getter = getter
        self.setter = setter
        self.min = min
        self.max = max 
        self.update()
       
    def update(self):
        value = self.getter()
        if value < self.min : value = self.min
        if value > self.max : value = self.max
        convertedValue = int((value-self.min)/float(self.max-self.min) * (self.slider.GetMax()-self.slider.GetMin()) + self.slider.GetMin())
        self.slider.SetValue(convertedValue)
        self.valueWidget.SetLabel( "%.2f" % value )
        
    def set(self):
        value = self.getSliderValue()
        self.setter(value)
        self.valueWidget.SetLabel( "%.2f" % value )
       
    def getSliderValue(self):
        convertedValue = self.slider.GetValue()
        return float((convertedValue-self.slider.GetMin())/float(self.slider.GetMax()-self.slider.GetMin()) * (self.max-self.min) + self.min)
        

class ToolsetBase(object):
    
    def __init__(self):
        """Abstract base class for toolsets that can contain options.
        The subclass must contain a self._toolSet member."""
        
        self._dataList = []
        
        

    def update(self, data = None):
        """Called whenever the animation is updated in the application."""
        for data in self._dataList:
            data.update()
            
        
    def addOption(self, label, getter, setter):
        """Adds a new option to the panel, with its getter and setter."""
        checkBox = self._toolSet.addTool( wx.CheckBox, label = label, size=(-1,22) )
        data = _OptionData(checkBox, getter, setter)
        checkBox.Bind( wx.EVT_CHECKBOX, lambda e: data.set() )
        self._dataList.append( data )

        return checkBox, data
                
    def addSlider(self, label, min, max, step, getter, setter, labelWidth = 75, valueWidth = 40):
        """Adds a new horizontal slider to the panel, with its getter and setter."""
        toolPanel = self._toolSet.addTool( wx.Panel )
        sizer = wx.BoxSizer( wx.HORIZONTAL )
        
        labelWidget = wx.StaticText( toolPanel, label = label, size=(labelWidth,-1) )
        slider = wx.Slider( toolPanel, size=(-1,22), style = wx.SL_HORIZONTAL, minValue = 0, maxValue = math.ceil(float(max-min)/float(step)) )
        slider.SetLineSize( step )
        valueWidget = wx.StaticText( toolPanel, label = "0", size=(valueWidth,-1) )

        sizer.Add(labelWidget, 0, wx.ALIGN_CENTER)
        sizer.Add(slider, 1, wx.EXPAND)
        sizer.Add(valueWidget, 0, wx.ALIGN_CENTER)
        toolPanel.SetSizerAndFit( sizer )

        data = _SliderData(slider, valueWidget, getter, setter, min, max)
        slider.Bind( wx.EVT_SCROLL, lambda e: data.set() )
        self._dataList.append( data )
        
        return slider, data
                
    def addButton(self, label, action):
        """Adds a new option button the panel, with its action (a parameter-less function)."""
        button = self._toolSet.addTool( wx.Button, label = label, flag=0 )
        button.Bind(wx.EVT_BUTTON, lambda event: action() )
        
        return button
