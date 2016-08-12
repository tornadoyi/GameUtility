# emacs-mode: -*- python-*-
#coding=utf-8
import os
import xlrd
import sys
import codecs

reload(sys)
sys.setdefaultencoding( "utf-8" )

keyLine = 2
typeLine = 3
readStartLine = typeLine + 1

configStartLine = 1

defaultTypeDict = {
	'string' 	: 'nil',
	'str' 		: 'nil',
	'int'		: '0',
	'double'	: '0.0',
	'float'		: '0.0',
	'bool'		: 'false'
}

def IsEmptyCell(v):
	if v == None: return True
	s = str(v)
	return len(s) <= 0

def OnTxtProcess(table):
	ret = ""
	nrows = table.nrows
	ncols = table.ncols

	for i in range(readStartLine, nrows):
		key = str(table.row_values(i)[0])
		value = str(table.row_values(i)[2])
		if len(key) == 0: continue
		ret = ret + key + " = " + value + "\n"

	return ret

def OnLuaProcess(table):
	ret = "local t = {\n"
	nrows = table.nrows
	ncols = table.ncols

	# Get headers
	headers = []
	for i in range(0, ncols):
		key = str(table.row_values(keyLine)[i]).lower()
		ktype = str(table.row_values(typeLine)[i]).lower()
		column = i

		# Skip comment header
		if IsEmptyCell(key) or ktype == "null": continue

		# Check type
		if defaultTypeDict[ktype] == None: 
			print("Unsupport type " + ktype) 
			continue

		# Save header
		headers.append( (key, ktype, column) )


	# Load all rows
	for i in range(readStartLine, nrows):
		# SKip empty line
		elements = table.row_values(i)
		if len(elements) < ncols: break
		
		# Collect line
		line = "{"
		for header in headers:
			key = header[0]
			ktype = header[1]
			column = header[2]

			# Save key
			line += key + " = " 

			# Save value
			value = elements[column]
			defaut = defaultTypeDict[ktype]

			if IsEmptyCell(value): line += defaut
			elif ktype == "string" or ktype == "str": line += '"' + str(value) + '"'
			elif ktype == "double" or ktype == "float": line += "%.2f" %float(value)
			elif ktype == "int": line += "%d" %int(value)
			elif ktype == "bool": 
				strBool = value.lower()
				if strBool != "true" and strBool != "false":
					print("Invalid bool value %s, line: %d column: %d" %value %i % column)
					return
				line += strBool
			else: 
				print("Unkown error") 
				return

			line += ", "	

		ret += line + "},\n"

	ret = ret + "}\nreturn t"
	return ret



def Export(relativeExcelFilePath, relativeOutputPath, outputType):
	# Check parms
	if  relativeExcelFilePath == None or relativeOutputPath == None or outputType == None:
		print("Invalid arguments")
		return

	# Get absolute path
	currentPath = os.getcwd()
	excelFilePath = os.path.join(currentPath, relativeExcelFilePath)
	outputFilePath = os.path.join(currentPath, relativeOutputPath)

	# Check excel file exist
	if not os.path.exists(excelFilePath): 
		print("Can not find excel path " + excelFilePath)
		return

	# Create ouput path, clean old output file
	outputDir = os.path.dirname(outputFilePath)
	if not os.path.exists(outputDir):
		os.makedirs(outputDir)	
	if os.path.exists(outputFilePath): os.remove(outputFilePath)

	# Open excel and output file
	fd = codecs.open(outputFilePath,'w','utf_8')
	data = xlrd.open_workbook(excelFilePath)

	# Process
	strs = None
	table = data.sheets()[0]
	if int(outputType) == 1:
		strs = OnLuaProcess(table)
	else:
		strs = OnTxtProcess(table)

	fd.write(strs)
	fd.close()

def ReadConfig(relativeConfigPath):
	# Check config file
	if relativeConfigPath == None:
		print("Invalid relativeConfigPath")
		return

	# Check exist
	currentPath = os.getcwd()
	excelFilePath = os.path.join(currentPath, relativeConfigPath)
	if not os.path.exists(excelFilePath): 
		print("Can not find config excel path " + excelFilePath)
		return

	# Open config
	data = xlrd.open_workbook(excelFilePath)
	table = data.sheets()[0]

	# Get config and export
	configs = {}
	nrows = table.nrows
	ncols = table.ncols
	for i in range(configStartLine, nrows):
		# Get path
		relativeExcelFilePath = str( table.row_values(i)[0] )
		relativeOutputPath = str( table.row_values(i)[1] )
		outputType = table.row_values(i)[2]

		# Skip empty lines
		if IsEmptyCell(relativeExcelFilePath) or IsEmptyCell(relativeOutputPath): continue
		if IsEmptyCell(outputType): 
			print("Need set export type for excel %s" %relativeExcelFilePath)
			continue

		# Export
		print("Export " + relativeExcelFilePath)
		Export(relativeExcelFilePath, relativeOutputPath, outputType)


def Main():
	ReadConfig('exporter.xlsx')

Main()










