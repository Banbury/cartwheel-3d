import MathLib

x = MathLib.Vector3d()
x.setValues(10,0.44,-132)
print "x = ",
x._print()
y = MathLib.Vector3d()
y.setValues(3,11,2)
print "y = ",
y._print()
x.addScaledVector(y,0.5)
print "x + 0.5y = ",
x._print()

