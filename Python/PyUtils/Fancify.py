'''
Created on 2009-09-08

@author: beaudoin

Contains a abstract syntactic tree visitor for printing
nested function calls in a nice way. Used for cleaning-up 
output of the serialize method.

See the python standard ast module for details.
'''

import ast

class Fancify( ast.NodeVisitor ):
    
    def __init__(self):
        super( Fancify, self ).__init__()        
        self._tabLevel = 0
        self._spacesPerTab = 4

    def visitListElements(self, elements):
        """Inserts commas between elements and will go multi-line if needed."""
        # Try single line
        content = ", ".join( elements ) + " "
        nbCR = content.count('\n')
        long = len(content) > 100  or  nbCR > 0
        veryLong = long and nbCR > 40
        if long:
            # Too long, go multi-line
            spacer = "\n" + " " * (self._tabLevel * self._spacesPerTab)
            if veryLong : spacer = "\n" + spacer
            content = spacer
            spacer = "," + spacer
            content += spacer.join( elements )
            if veryLong :
                content += "\n" + " " * ((self._tabLevel-1) * self._spacesPerTab)
            else :
                content += " "
        return content

    def visitListType(self, list):
        """Visits anything that looks like a list of AST nodes.""" 
        self._tabLevel += 1
        elements = map( self.visit, list ) 
        result = self.visitListElements( elements )
        self._tabLevel -= 1
        return result
    
    def visit_Call(self, node):
        """Fancify a function a call or an object creation."""
        assert node.starargs is None and node.kwargs is None, "Star arguments not supported by fancify"
        return self.visit(node.func) + "( " + self.visitListType( node.args + node.keywords ) + ")"
        
    def visit_List(self, node):
        """Fancify a list."""
        return "[ " + self.visitListType( node.elts ) + "]"
        
    def visit_Dict(self, node):
        """Fancify a dictionary."""
        self._tabLevel += 1
        keys = map( self.visit, node.keys )
        values = map( self.visit, node.values )
        elements = []
        for key, value in zip(keys,values):
            elements.append( key + ' : ' + value )        
        result = "{ " + self.visitListElements( elements ) + "}"
        self._tabLevel -= 1
        return result
        
    def visit_Tuple(self, node):
        """Fancify a tuple."""
        return "( " + self.visitListType( node.elts ) + ")"
        
    def visit_Name(self, node):
        """Visits a simple identifier"""
        return node.id
    
    def visit_Attribute(self, node):
        """Visits an 'object.attribute' node"""
        return self.visit(node.value) + '.' + node.attr
        
    def visit_keyword(self, node):
        """Fancify a keyword within a function a call or an object creation."""
        return str(node.arg) + " = " + self.visit( node.value )
    
    def visit_Str(self, node):
        """Fancify a simple string"""
        return repr(node.s)
    
    def visit_Num(self, node):
        """Fancify a number"""
        return str(node.n)
    
    def generic_visit(self, node):
        return "\n".join( map( self.visit, ast.iter_child_nodes(node) ) )

        
    