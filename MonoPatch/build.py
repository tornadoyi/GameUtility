import os
import io
import sys
import hashlib
import getopt
import shutil
import codecs

def docmd(cmd):
	print(cmd)
	os.system(cmd)


def copy_files(src, dst):
	if not os.path.isdir(src): print("error: %s is not path" % (src)); return
	if not os.path.isdir(dst): print("error: %s is not path" % (dst)); return

	for f in os.listdir(src): 
		srcf = os.path.join(src, f)
		dstf = os.path.join(dst, f)

		# file then copy to dst
		if os.path.isfile(srcf):
			shutil.copy(srcf, dstf)
		else:
			if not os.path.exists(dstf): os.makedirs(dstf) 
			copy_files(srcf, dstf)

def search(workspace, usefilter, unusedfilter, process):
	# search files
	for dirpath, dirnames, filenames in os.walk(workspace):
		for filename in filenames:
			splits = os.path.splitext(filename)
			ext = splits[len(splits)-1]
			if usefilter != None and usefilter.find(ext) < 0: continue
			if unusedfilter != None and usefilter.find(ext) >= 0: continue
			process(dirpath, filename)

def git_reset():
	docmd("git reset --hard")
	docmd("git clean -fd ./")



def usage():
	print("Usage:")
	print("-h [--help]                                      : manual")
	print("-m [--mono=] PATH                                : set mono path")
	print("-p [--patch=] PATH                               : set patch path")
	print("-k [--key=] ENCRYPT KEY                          : ecrypt key")
	print("-s [--section=] SECTION NAME                     : ecrypt section name")
	print("-e [--encrypt-only]                              : ecrypt only")

def main():
	patchpath = os.path.abspath("patch")
	monopath = os.path.abspath("mono")
	key = "d2a460df08a4fb7a558f635b540d90cb"
	section = ".encryptsection"
	encrypt_only = False

	try:
		options,args = getopt.getopt(sys.argv[1:], "hem:p:k:s:", ["help", "--encrypt-only", "mono=", "patch=", "key=", "section="])
	except getopt.GetoptError:
		usage()
		return

	for name, value in options:
		if name in ("-h","--help"):
			usage()
			return

		elif name in ("-m","--mono"):
			monopath = os.path.abspath(value)

		elif name in ("-p","--patch"):
			patchpath = os.path.abspath(value)

		elif name in ("-k","--key"):
			key = value

		elif name in ("-s","--section"):
			section = value

		elif name in ("-e","--encrypt-only"):
			encrypt_only = True

	if key == None or section == None:
		print(key)
		print(section)
		usage()
		return

	def build():
		# reset git
		os.chdir("mono")
		git_reset()
		os.chdir("../")
			
		# copy files
		copy_files(patchpath, monopath)

		# build
		os.chdir("mono")
		docmd("external/buildscripts/build_runtime_android.sh")
		os.chdir("../")

		# copy so
		docmd("rm -rf android")
		docmd("cp -rf mono/builds/embedruntimes/android ./")

		# reset git
		os.chdir("mono")
		git_reset()
		os.chdir("../")

	def encrypt():
		# encrypt so
		docmd("rm -rf android-encrypt")
		docmd("cp -rf android ./android-encrypt")
		encryptpath = os.path.abspath("android-encrypt")
		def encrypt(dirpath, filename):
			filepath = os.path.join(dirpath, filename); 
			docmd("./encrypt %s %s %s" % (filepath, section, key))
		
		search(encryptpath, ".so", None, encrypt)

	if encrypt_only == True:
		encrypt()
	else:
		build()
		encrypt()

main()
