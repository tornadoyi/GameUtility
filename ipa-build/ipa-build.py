#!/usr/bin/python
import argparse
import os
import sys
import subprocess
import shutil

# config
def get_project_path(): return os.path.split( os.path.realpath( sys.argv[0] ) )[0]

def get_resign_shell_path(): return os.path.join( get_project_path(), "resign_ipa.sh")

def get_inhouse_provision_path(path): return os.path.join( get_project_path(), path)

def get_file_full_path(relativePath):
	if relativePath[0] == '/': return relativePath
	return os.path.join( os.getcwd(), relativePath)

def search_xcodeproj(path):
	for file in os.listdir(path):
		filepath = os.path.join(path, file)
		if not os.path.isdir(filepath): continue
		ext = os.path.splitext(filepath)[-1]
		if ext != '.xcodeproj': continue
		return filepath
	raise Exception('xcodeproj can not find in {}'.format(path))




def unlock_keychain(password):
	cmd = "security unlock-keychain -p {} ~/Library/Keychains/login.keychain".format(password)
	print(cmd)
	os.system(cmd)

def clean_project(project_path, configuration=None):
	project_file = search_xcodeproj(project_path)
	if configuration is None: configuration = "Release"

	cmd = "xcodebuild clean "
	cmd += "-project \"{}\" ".format(project_file)
	cmd += "-configuration {} ".format(configuration)
	cmd += "-alltargets "
	print(cmd)
	os.system(cmd)


def archive_project(project_path, scheme, password, configuration, archive_path):
	project_file = search_xcodeproj(project_path)
	if configuration is None: configuration = "Release"
	if archive_path is None: archive_path = project_path
	archive_path = archive_path.strip()
	ext = os.path.splitext(archive_path)[-1]
	if ext != ".xcarchive": archive_path = os.path.join(archive_path, "{}.xcarchive".format(scheme))

	cmd = "xcodebuild archive "
	cmd += "-project \"{}\" ".format(project_file)
	cmd += "-scheme \"{}\" ".format(scheme)
	cmd += "-archivePath \"{}\" ".format(archive_path)
	cmd += "-configuration {} ".format(configuration)

	# Need to unlock keychain for codesign in archive process 
	unlock_keychain(password)

	# Archive
	print(cmd)
	os.system(cmd)


def export_ipa(xarchive, export_path, export_options):
	cmd = "xcodebuild "
	cmd += "-exportArchive "
	cmd += "-archivePath \"{}\" ".format(xarchive)
	cmd += "-exportPath \"{}\" ".format(export_path)
	cmd += "-exportOptionsPlist \"{}\" ".format(export_options)

	# clean ipa
	if os.path.exists(export_path): os.remove(export_path)

	# run command
	print(cmd)
	os.system(cmd)

def resign_ipa(input_ipa_path, output_ipa_path, provision_path, password):
	# clean dst ipa
	if os.path.exists(output_ipa_path): os.remove(output_ipa_path)

	# copy src to dst
	shutil.copy(input_ipa_path, output_ipa_path)

	# unlock keychain
	unlock_keychain(password)

	# run resign command
	resign_shell = get_resign_shell_path()
	cmd = 'sh \"{}\" \"{}\" \"{}\"'.format(resign_shell, output_ipa_path, provision_path)
	print(cmd)
	os.system(cmd)



def do(args):
	# check params
	def check_param(name, default=None):
		v = args.__dict__.get(name)
		if v is not None: return v
		if default is not None: return default
		raise Exception('Error: {} need argument {}'.format(args.command, name))

	# dispatch command
	if args.command == "clean":
		project = check_param('project')
		clean_project(get_file_full_path(project), args.configuration)

	elif args.command == "archive":
		project = get_file_full_path(check_param('project'))
		scheme = check_param('scheme')
		keychain_password = check_param('keychain_password')
		configuration = check_param('configuration', 'Release')
		output_archive = check_param("output_archive", project)
		archive_project(project, scheme, keychain_password, configuration, output_archive)

	elif args.command == "export":
		xarchive = get_file_full_path(check_param('xarchive'))
		export_path = get_file_full_path(check_param('export_path'))
		export_options = check_param('export_options')
		export_ipa(xarchive, export_path, export_options)

	elif args.command == "resign":
		input_ipa = get_file_full_path(check_param('input_ipa'))
		output_ipa = get_file_full_path(('output_ipa'))
		provision_file = get_file_full_path(check_param('provision_file'))
		keychain_password = check_param('keychain_password')
		resign_ipa(input_ipa, output_ipa, provision_file, keychain_password)

	else:
		raise Exception('invalid action {}'.format(args.command))


if __name__ == "__main__":
	example = "Examples: \n"
	example += "	ipa-build clean --project=/root/example \r\n"
	example += "	ipa-build archive --project=/root/example --scheme=Example --keychain-password=xxxxxx \n"
	example += "	ipa-build export --xarchive=/root/example/Example.xcarchive --export-path=/tmp/Example-Export " \
			   "--export-options=/root/example/ExportOptions.plist\n"
	example += "	ipa-build resign --input-ipa=/tmp/Example-Export/Example.ipa --output-ipa=/tmp/Example-Export/Example.resign.ipa " \
			   "--provision-file=~/Library/MobileDevice/Provisioning Profiles/xxx.mobileprovision --keychain-password=xxxx \n"

	def str2bool(v): return v.lower() in ("yes", "true", "t", "1", True)

	parser = argparse.ArgumentParser(prog="ipa-build", description="Run commands")
	parser.add_argument('command', choices=['clean', 'archive', 'export', 'resign'], help='please choose action')
	parser.add_argument('--project', type=str, help='a xcode project for build')
	parser.add_argument('--xarchive', type=str, help='a xcode xarchive for export')
	parser.add_argument('--keychain-password', type=str, help='a password for unlock keychain')
	parser.add_argument('--input-ipa', type=str, help='ipa package path for input')
	parser.add_argument('--output-ipa', type=str, help='ipa package path for output')
	parser.add_argument('--scheme', type=str, help='scheme is used for archive')
	parser.add_argument('--configuration', type=str, help='a configuration used for clean and archive, see xcodebuid -list')
	parser.add_argument('--output-archive', type=str, help='a path xarchive will be saved in')
	parser.add_argument('--export-options', type=str, help='a plist of export options for export')
	parser.add_argument('--provision-file', type=str, help='a mobile provision file be used for resign')
	parser.add_argument('--export-path', type=str, help='a path for export ipa')

	# parse args
	args = parser.parse_args()

	# do
	try:
		do(args)

	except Exception as e:
		parser.print_help()
		print(example)
		print(e)

