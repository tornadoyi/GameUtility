import os
import io
import sys
import hashlib
import getopt
import shutil
import codecs


# config
parsefilename = "parse.log"
translatefilename = "translate.log"

#reload(sys)
#sys.setdefaultencoding( "utf-8" )

class LocalizeLine:
	def __init__(self, linenumber, texts):
		self.linenumber = linenumber
		self.texts = texts

class LocalizeFile:
    def __init__(self, filepath, lines):
        self.filepath = filepath   
        self.lines = lines 


def project_path(): return os.path.split( os.path.realpath( sys.argv[0] ) )[0]

def char_in_ascii_range(uchar):
	if uchar > u'\u00ff':
		return False
	else:
		return True

def str_in_ascii_range(ustring):
	for uchar in ustring:
		if char_in_ascii_range(uchar): continue
		return False
	return True


def search(workspace, usefilter, unusedfilter, process):
	# search files
	for dirpath, dirnames, filenames in os.walk(workspace):
		for filename in filenames:
			splits = os.path.splitext(filename)
			ext = splits[len(splits)-1]
			if usefilter != None and usefilter.find(ext) < 0: continue
			if unusedfilter != None and usefilter.find(ext) >= 0: continue
			process(dirpath, filename)

def get_localization_txt(filepath, inquotation):

	def _split_quotation_mark(line, number):
		# find all quotation marks
		length = len(line)
		locs = []
		i = 0
		while True:
			if i >= length: break
			pos = line.find('\"', i, length)
			if pos < 0: break
			locs.append(pos)
			i = pos + 1

		# check count of quotation mark
		if len(locs) == 0: return []
		elif len(locs) % 2 != 0:
			print("error: odd number in [%s: %d]: %s" % (filepath, number, line))
			return []

		# pick all content
		ret = []
		i = 0
		while True:
			if i >= len(locs): break
			st = locs[i]
			ed = locs[i+1]
			txt = line[st+1:ed]
			i = i + 2
			if str_in_ascii_range(txt): continue
			#print(txt)
			
			ret.append(txt)
		return ret


	def _split_not_ascii(line, linenumber):
		# find all quotation marks
		length = len(line)
		ret = []
		i = -1
		st = None
		for uchar in line:
			i = i + 1

			# set start of text
			if not char_in_ascii_range(uchar):
				if st == None: st = i
				continue

			# split text
			if st == None: continue
			txt = line[st:i]
			ret.append(txt)
			st = None

		return ret

	fd = codecs.open(filepath, 'r', encoding='utf-8')
	lines = []
	try:
		for line in fd: lines.append(line)
	except:
		print("can not read " + filepath)
	fd.close()

	locallines = []
	ln = 1
	for line in lines:
		txts = []
		if inquotation == True: txts = _split_quotation_mark(line, ln)
		else: txts = _split_not_ascii(line, ln)
		curline = ln
		ln = ln + 1
		if len(txts) == 0: continue
		localline = LocalizeLine(curline, txts)
		locallines.append(localline)
		

	return locallines


def load_symbol_file(symbolpath):
	symboldict = {}
	keydict = {}

	# read all lines
	fd = codecs.open(symbolpath, 'r', encoding='utf-8')
	lines = []
	needkey = False
	scount = 0
	for line in fd: 
		if len(line) == 0: continue 
		lines.append(line)
		if line.find(':') > 0: scount = scount + 1
	fd.close()
	if scount / len(lines) > 0.618: needkey = True

	# parse
	linenumber = 0
	for line in lines:
		linenumber = linenumber + 1
		key = None
		kpos = -1
		if needkey:
			kpos = line.find(':')
			if kpos < 0: print("Error: invalid symbol line %s, missing " % (linenumber)); continue
			key = line[0:kpos].rstrip()

		pos = line.find('=', kpos+1)
		if pos < 0: print("Error: invalid symbol line %s, missing =" % (linenumber)); continue
		k = line[kpos+1:pos].rstrip()
		v = line[pos+1:len(line)].lstrip().rstrip('\n')
		symboldict[k] = v
		keydict[k] = key
		#print("key:%s origin:%s symbol:%s" %(key, k, v))

	return symboldict, keydict


def parse_core(workspace, usefilter, unusedfilter, inquotation):
	# core
	localfiles = []
	def do(dirpath, filename):
		filepath = os.path.join(dirpath, filename); 
		locallines = get_localization_txt(filepath, inquotation)
		if len(locallines) == 0: return
		localfile = LocalizeFile(filepath, locallines)
		localfiles.append(localfile)


	# search
	search(workspace, usefilter, unusedfilter, do)

	return localfiles

def parse(workspace, outputpath, usefilter, unusedfilter, detail, inquotation):
	# check
	if not os.path.exists(workspace): print("can not find workspace %s" % (workspace)); return None
	if not os.path.exists(outputpath): print("can not find output path %s" % (outputpath)); return None

	# parse
	localfiles = parse_core(workspace, usefilter, unusedfilter, inquotation)

	# output file
	outputFilePath = os.path.join(outputpath, parsefilename)
	if os.path.exists(outputFilePath): os.remove(outputFilePath)
	fd = codecs.open(outputFilePath, 'w', encoding='utf-8')

	for localfile in localfiles:
		for localline in localfile.lines:
			for text in localline.texts:
				if detail == True:
					fd.write("[%s: %d]: %s\n" % (localfile.filepath, localline.linenumber, text))
				else:
					fd.write(text + '\n')

	fd.close()


def translate(workspace, outputpath, symbolpath, usefilter, unusedfilter, detail, inquotation, replace):
	# check
	if not os.path.exists(workspace): print("can not find workspace %s" % (workspace)); return None
	if not os.path.exists(outputpath): print("can not find output path %s" % (outputpath)); return None
	if not os.path.exists(symbolpath): print("can not find symbol file path %s" % (symbolpath)); return None

	# load symbol path
	symboldict, keydict = load_symbol_file(symbolpath)
	
	# parse
	localfiles = parse_core(workspace, usefilter, unusedfilter, inquotation)

	# replace
	unsymbols = []
	for localfile in localfiles:
		# read all lines
		fd = codecs.open(localfile.filepath, 'r', encoding='utf-8')
		lines = []
		for line in fd: lines.append(line)
		fd.close()

		# write
		fd = codecs.open(localfile.filepath, 'w+', encoding='utf-8')
		for localline in localfile.lines:
			for text in localline.texts:
				# can not find in symbol dict
				if not symboldict.has_key(text):
					if detail == True:
						unsymbols.append("[%s: %d]: %s\n" % (localfile.filepath, localline.linenumber, text))
					else:
						unsymbols.append(text)
					continue

				newtext = None
				if replace == None:
					newtext = symboldict[text]
				else:
					key = keydict[text]
					newtext = replace.format(key)

				# replace
				line = lines[localline.linenumber-1]
				if replace != None and inquotation == True:
					lines[localline.linenumber-1] = line.replace('\"%s\"' % (text), newtext)
				else:
					lines[localline.linenumber-1] = line.replace(text, newtext)

				#print("%s ==> %s" % (text, newtext))

		fd.writelines(lines)
		fd.close()

	for err in unsymbols:
		try:
			print("can not find symbol: " + err)
		except Exception, e:
			print("can not decode")


def check(workspace, outputpath, symbolpath, usefilter, unusedfilter, detail, inquotation):
	# check
	if not os.path.exists(workspace): print("can not find workspace %s" % (workspace)); return None
	if not os.path.exists(outputpath): print("can not find output path %s" % (outputpath)); return None
	if not os.path.exists(symbolpath): print("can not find symbol file path %s" % (symbolpath)); return None

	# load symbol path
	symboldict, keydict = load_symbol_file(symbolpath)
	
	# parse
	localfiles = parse_core(workspace, usefilter, unusedfilter, inquotation)

	# check
	unsymbols = []
	for localfile in localfiles:
		# read all lines
		fd = codecs.open(localfile.filepath, 'r', encoding='utf-8')
		lines = []
		for line in fd: lines.append(line)
		fd.close()

		# check
		for localline in localfile.lines:
			for text in localline.texts:
				# can not find in symbol dict
				if not symboldict.has_key(text):
					if detail == True:
						unsymbols.append("[%s: %d]: %s\n" % (localfile.filepath, localline.linenumber, text))
					else:
						unsymbols.append(text)
					

	for err in unsymbols:
		try:
			print("can not find symbol: " + err)
		except Exception, e:
			print("can not decode")

def usage():
	print("Usage:")
	print("parse -w [WORKSOACE] -o [OUTOUT]                 : parse file")
	print("translate -w [WORKSOACE] -o [OUTOUT] -s [FILE]   : translate file")
	print("check -w [WORKSOACE] -o [OUTOUT] -s [FILE]   	: check file")
	print("                                                ")
	print("-h [--help]                                      : manual")
	print("-w [--workspace=] PATH                           : set workspace")
	print("-o [--output=] PATH                              : set output")
	print("-u [--useext=] FILE EXTERNSIONS                  : file extensions will be used")
	print("-n [--unusedext=] FILE EXTERNSIONS               : unused file extensions")
	print("-s [--symbol=] FILE PATH                         : symbol file for translate")
	print("-d [--detail]                                    : output detail infomation")
	print("-q [--inquotation]                               : find text in quotation mark")
	print("-r [--replace]                               	: replace content for symbols")



def main():
	# check
	if len(sys.argv) < 2:
		usage()
		return

	# parse command
	cmdname = sys.argv[1].lower()
	

	# parse option
	workspace = None
	outputpath = None
	usefilter = None
	unusedfilter = None
	symbolpath = None
	detail = False
	inquotation = False
	replace = None

	try:
		options,args = getopt.getopt(sys.argv[2:], "hdqw:o:u:n:s:r:", ["help", "detail", "inquotation", "workspace=", "output=", "useext=", "unusedext=", "symbol=", "replace="])
	except getopt.GetoptError:
		usage()
		return

	for name, value in options:
		if name in ("-h","--help"):
			usage()
			return

		elif name in ("-w","--workspace"):
			workspace = os.path.abspath(value)

		elif name in ("-o","--output"):
			outputpath = os.path.abspath(value)

		elif name in ("-u","--useext"):
			usefilter = value

		elif name in ("-n","--unusedext"):
			unusedfilter = value

		elif name in ("-s","--symbol"):
			symbolpath = os.path.abspath(value)

		elif name in ("-d","--detail"):
			detail = True

		elif name in ("-q","--inquotation"):
			inquotation = True

		elif name in ("-r","--replace"):
			replace = value

	if cmdname == "help":
		usage()

	elif cmdname == "parse":
		if workspace == None or outputpath == None: usage(); return
		parse(workspace, outputpath, usefilter, unusedfilter, detail, inquotation)
			
	elif cmdname == "translate":
		if workspace == None or outputpath == None or symbolpath == None: usage(); return
		translate(workspace, outputpath, symbolpath, usefilter, unusedfilter, detail, inquotation, replace)

	elif cmdname == "check":
		if workspace == None or outputpath == None or symbolpath == None: usage(); return
		check(workspace, outputpath, symbolpath, usefilter, unusedfilter, detail, inquotation)

	else:
		print("unkown command " + cmdname)
		usage()
main()	