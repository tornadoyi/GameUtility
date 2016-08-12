#!/usr/bin/python
import os
import sys
import subprocess
import shutil

# config
keychain_unlock_passwd = "10166101"

def GetProjectPath(): return os.path.split( os.path.realpath( sys.argv[0] ) )[0]

def GetResignShellPath(): return os.path.join( GetProjectPath(), "resign_ipa.sh")

def GetInhouseProvisionPath(): return os.path.join( GetProjectPath(), "Certificate/HEInHouseProvision.mobileprovision")

def GetFileFullPath(relativePath):
	if relativePath[0] == '/': return relativePath
	return os.path.join( os.getcwd(), relativePath)

def UnlockKeyChain(password):
	cmd = "security unlock-keychain \
-p %s \
~/Library/Keychains/login.keychain \
" % (password)
	print(cmd)
	os.system(cmd)

def CleanProject(projectPath):
	cmd = "xcodebuild clean \
-project %s/Unity-iPhone.xcodeproj \
-configuration Release \
-alltargets\
" % (projectPath)
	print(cmd)
	os.system(cmd)


def ArchiveProject(projectPath, password):
	cmd = "xcodebuild archive \
-project %s/Unity-iPhone.xcodeproj \
-scheme Unity-iPhone \
-archivePath %s/Unity-iPhone.xcarchive\
" % (projectPath, projectPath)
	
	# Need to unlock keychain for codesign in archive process 
	UnlockKeyChain(password)

	# Archive
	print(cmd)
	os.system(cmd)

def ExportIPA(projectPath, provisionName, ipaPath):
	cmd = "xcodebuild \
-exportArchive \
-archivePath %s/Unity-iPhone.xcarchive \
-exportPath %s \
-exportFormat ipa \
-exportProvisioningProfile %s \
" % (projectPath, ipaPath, provisionName)

	# clean ipa
	if os.path.exists(ipaPath): os.remove(ipaPath)

	# run command
	print(cmd)
	os.system(cmd)

def ResignIPA(srcIPAPath, dstIPAPath, provisionPath, keychainPassword):
	# clean dst ipa
	if os.path.exists(dstIPAPath): os.remove(dstIPAPath)

	# copy src to dst
	shutil.copy(srcIPAPath, dstIPAPath)

	# unlock keychain
	UnlockKeyChain(keychainPassword)

	# run resign command
	resignShell = GetResignShellPath()
	cmd = "sh %s %s %s" % (resignShell, dstIPAPath, provisionPath)
	print(cmd)
	os.system(cmd)


def Help():
	print("Usage: ipa-build [clean | archive | resign]")
	print("Usage: ipa-build clean [project]")
	print("Usage: ipa-build archive [project] [keychain password (optional)]")
	print("Usage: ipa-build export [project] [ipa name] [provision name]")
	print("Usage: ipa-build resign [src ipa] [dst ipa] [provision file path] [keychain password (optional)]")
	print("Usage: ipa-build inhouse [src ipa] [dst ipa] [keychain password (optional)]")
	print("Usage: ipa-build all [project] [ipa name] [inhouse ipa name] [provision name]")

def Main():
	# check build type parameter
	if len(sys.argv) < 2: Help(); return
		
	# check build type
	buildType = sys.argv[1].lower()
	if buildType == "clean":
		if len(sys.argv) < 3: Help(); return
		projectPath = GetFileFullPath(sys.argv[2])
		CleanProject(projectPath)

	elif buildType == "archive":
		if len(sys.argv) < 3: Help(); return
			
		projectPath = GetFileFullPath(sys.argv[2])
		unlockPwd = len(sys.argv) >= 4 and sys.argv[3] or keychain_unlock_passwd
		ArchiveProject(projectPath, unlockPwd)

	elif buildType == "export":
		if len(sys.argv) < 5: Help(); return
		projectPath = GetFileFullPath(sys.argv[2])
		ipaPath = GetFileFullPath(sys.argv[3])
		provisionName = sys.argv[4]

		ExportIPA(projectPath, provisionName, ipaPath)

	elif buildType == "resign":
		if len(sys.argv) < 5: Help(); return
		srcIPAPath = GetFileFullPath(sys.argv[2])
		dstIPAPath = GetFileFullPath(sys.argv[3])
		provisionPath = GetFileFullPath(sys.argv[4])
		unlockPwd = len(sys.argv) >= 6 and sys.argv[5] or keychain_unlock_passwd
		ResignIPA(srcIPAPath, dstIPAPath, provisionPath, unlockPwd)

	elif buildType == "inhouse":
		if len(sys.argv) < 4: Help(); return
		srcIPAPath = GetFileFullPath(sys.argv[2])
		dstIPAPath = GetFileFullPath(sys.argv[3])
		provisionPath = GetInhouseProvisionPath()
		unlockPwd = len(sys.argv) >= 5 and sys.argv[4] or keychain_unlock_passwd
		ResignIPA(srcIPAPath, dstIPAPath, provisionPath, unlockPwd)

	else:
		print("Invalid command " + sys.argv[1])
		Help()

Main()