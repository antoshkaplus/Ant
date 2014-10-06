
import os
import sys

executable = (
    "/Users/antoshkaplus/Documents"
    "/Programming/Learning/C++/Ant"
    "/DerivedData/Ant/Build/Products/Debug/TestAnt"
)

data_file = sys.argv[1]
os.system(executable + " < " + data_file)
