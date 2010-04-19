'''
Created on 2010-04-19

@author: Philippe Beaudoin
'''
from distutils.core import setup
import py2exe

setup(windows=['BasicEditor.py'],
      options={
          "py2exe": {
              "includes": ["ctypes", "logging"],
              "excludes": ["OpenGL"],
              }
          }
      )