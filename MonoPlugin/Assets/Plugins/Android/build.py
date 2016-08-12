
#!/usr/bin/python

import os
import sys
import subprocess
import shutil

name = "mono"

def GetProjectPath(): return os.path.split( os.path.realpath( sys.argv[0] ) )[0]

def Main():
	projectPath = GetProjectPath()
	os.system('android update project -n %s -p \"%s\"' % (name, projectPath))
	os.system("ant release")
	shutil.copy(os.path.join(projectPath, "bin/%s-release.apk" % (name)), 
				os.path.join(projectPath, "%s.apk" % (name)))

Main()
