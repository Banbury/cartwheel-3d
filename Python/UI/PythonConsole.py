'''
Created on 2009-08-30

@author: beaudoin

This file contains a class that can be used as a python interpreter console
'''

import wx
import sys

class PythonConsole(wx.Panel):
    """A console to output python and a command line interprete.
    You should only have one of this as it seize the standard output.
    """

    def __init__(self, parent, id = wx.ID_ANY, pos=wx.DefaultPosition,
                 size=wx.DefaultSize, style=wx.TAB_TRAVERSAL,
                 name='', consoleEnvironment={} ):
        """Initialize a console. You can pass other dictionnaries as globals and locals."""

        super(PythonConsole, self).__init__(parent, id, pos, size, style, name)
        
        #
        # Create the console components
        self._console = wx.TextCtrl(self, style=wx.TE_MULTILINE | wx.TE_READONLY | wx.TE_RICH2 | wx.TE_CHARWRAP )
        self._console.SetDefaultStyle( wx.TextAttr(wx.BLACK, wx.WHITE, wx.Font(8, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL) ) )
        self._commandLine = wx.TextCtrl(self, size=(-1,20), style = wx.TE_PROCESS_ENTER | wx.TE_RICH2 )
        self._commandLine.SetDefaultStyle( wx.TextAttr(wx.BLUE, wx.WHITE, wx.Font(8, wx.FONTFAMILY_TELETYPE, wx.FONTSTYLE_NORMAL, wx.FONTWEIGHT_NORMAL) ) )

        #
        # Create the sizer
        self._vBox = wx.BoxSizer(wx.VERTICAL)
        self._vBox.Add( self._console, 1, wx.EXPAND )        
        self._vBox.Add( self._commandLine, 0, wx.EXPAND )
        
        self.SetSizer( self._vBox )
        
        #
        # Do bindings
        self._commandLine.Bind( wx.EVT_KEY_DOWN, self.processKeyDown )
        self._commandLine.Bind( wx.EVT_TEXT_ENTER, self.processUserCommand )
        
        #
        # Initialize
        self._history = []
        self._historyIndex = 0
        self._currentCommand = u""
        self._commandDepth = 0
        self._runningCommand = u""
        self._globals = consoleEnvironment
        self._locals = consoleEnvironment
        sys.stdout = self


    #
    # write function for stdout and stderr 

    def write(self, text, color = None):
     
        # Freeze the window then scroll it as many lines as needed
        self._console.Freeze()
        before = self._console.GetLastPosition()
        self._console.AppendText( text )
        after = self._console.GetLastPosition()
        if color is not None :
            self._console.SetStyle (before, after, wx.TextAttr(color))
        self._console.ScrollLines( text.count('\n') + 1 )
        self._console.ShowPosition( self._console.GetLastPosition() )
        self._console.Thaw()
        

    #
    # wxPython Window Handlers
    
    def processKeyDown(self, event):
        """Called whenever the user presses a key in the command line.
        Used to catch the UP arrow and DOWN arrow and navigate through history.
        """
        
        keyCode = event.GetKeyCode()
        if keyCode == wx.WXK_UP:
            if self._historyIndex != 0:
                if self._historyIndex == len( self._history ):
                    self._currentCommand = self._commandLine.GetValue()
                self._historyIndex -= 1
                self._commandLine.SetValue( self._history[self._historyIndex] )
                self._commandLine.SetInsertionPointEnd()
        elif keyCode == wx.WXK_DOWN:
            if self._historyIndex  != len( self._history ):
                self._historyIndex += 1
                if self._historyIndex == len( self._history ):
                    self._commandLine.SetValue( self._currentCommand )
                else:
                    self._commandLine.SetValue( self._history[self._historyIndex] )
                self._commandLine.SetInsertionPointEnd()
        else:
            # Assume text was entered
            self._historyIndex = len( self._history )
            event.Skip()
    
    def processUserCommand(self, event):
        """Called when the user press enter to validate a new command line."""
        
        commandLine = self._commandLine.GetValue().strip()
        if len(commandLine) > 0:
            self._history.append( commandLine )
        self._historyIndex = len(self._history)
        self._currentCommand = u""

        if len(commandLine) == 0 and self._commandDepth > 0:
            self._commandDepth -= 1
            if self._commandDepth > 0:
                self.write( "... " + "    " * self._commandDepth + "<<<<\n", wx.BLUE )
            else:
                self.write( "<<<\n", wx.BLUE )
        else:
            if self._commandDepth == 0:
                self.write( ">>> ", wx.BLUE )
            else:
                self.write( "... ", wx.BLUE )
            commandLine = "    " * self._commandDepth + commandLine
            self._runningCommand += commandLine + "\n"
            self.write( commandLine + "\n", wx.BLUE )
            
            if commandLine.endswith(':'):
                self._commandDepth += 1

        if self._commandDepth == 0:
            # First assume it is an expression
            try:
                x = eval( self._runningCommand, self._globals, self._locals )
                if x is not None:
                    self._globals["_"] = x
                    print x
            # If it fails, execute it as a statement
            except:
                try:
                    exec self._runningCommand in self._globals, self._locals
                except NameError as exception:
                    self.write( "Symbol not found: " + str(exception) + "\n", wx.RED );
                except SyntaxError as exception:
                    if exception.text is not None:
                        self.write( "Syntax error: " + "\n", wx.RED );
                        self.write( "   " + exception.text + "\n", wx.RED );
                        self.write( "  " + " " * exception.offset + "^" + "\n", wx.RED );
                    else:
                        self.write( "Syntax error: " + str(exception) + "\n", wx.RED );
                except EnvironmentError as exception:
                    self.write( "IO error (" + str(exception.errno) + "): " + exception.strerror + "\n", wx.RED );
                except ArithmeticError as exception:
                    self.write( "Arithmetic error: " + str(exception) + "\n", wx.RED );
                except ImportError as exception:
                    self.write( "Import error: " + str(exception) + "\n", wx.RED );
                except AssertionError as exception:
                    self.write( "Assertion failed: " + str(exception) + "\n", wx.RED );
                except AttributeError as exception:
                    self.write( "Attribute not found: " + str(exception) + "\n", wx.RED );
                except TypeError as exception:
                    self.write( "Invalid type: " + str(exception) + "\n", wx.RED );
                except ValueError as exception:
                    self.write( "Invalid value: " + str(exception) + "\n", wx.RED );
                except IndexError as exception:
                    self.write( "Index out of range: " + str(exception) + "\n", wx.RED );
                except KeyError as exception:
                    self.write( "Key does not exist: " + str(exception) + "\n", wx.RED );
                except SystemError as exception:
                    self.write( "Internal error: " + str(exception) + "\n", wx.RED );
                except MemoryError as exception:
                    self.write( "Out of memory error: " + str(exception) + "\n", wx.RED );
                except SystemExit:
                    wx.GetApp().GetTopWindow().Close()
                except:
                    self.write( "Unknown error, exception raised: ", sys.exc_info()[0] + "\n", wx.RED );

            finally:
                self._runningCommand = u""                

        self._commandLine.SetValue(u"")
                