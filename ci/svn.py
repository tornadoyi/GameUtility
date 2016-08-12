# emacs-mode: -*- python-*-
#coding=utf-8
import os
import platform

def svn_bin():
    system = platform.system()
    if 'Windows' in system: 
        return "svn.exe"
    else:
        return "svn"

def version_info(filepath):

    def line_value(line):
        strs = line.split(": ")
        return key, value

    info = {}
    cmd = "%s info %s" % (svn_bin(), filepath)
    lines = os.popen(cmd).readlines()

    for line in lines:
        strs = line.split(": ")
        if(len(strs) < 2): continue
        k = strs[0].lower().replace(' ', '_')
        v = strs[1].strip('\n')
        info[k] = v
        

    return info


def commit_all(svnpath, username = None, password = None):
    def parse(line):
        index = line.find(' ')
        operator = line[0:index]
        path = line[index:len(line)].strip(' ').rstrip('\n')
        return operator, path

    cmd = "%s status %s" % (svn_bin(), svnpath)
    lines = os.popen(cmd).readlines()


    for line in lines:
        operator, path = parse(line)

        if '?' in operator:
            cmd = "%s add \"%s\"" %(svn_bin(), path)
            print(cmd)
            os.system(cmd)

        elif '!' in operator:
            cmd = "%s del \"%s\"" %(svn_bin(), path)
            print(cmd)
            os.system(cmd)

        elif 'M' in operator or 'm' in operator or \
            'A' in operator or 'a' in operator or \
            'D' in operator or 'd' in operator:
            pass

        else:
            print("unkown operator %s %s" % (operator, path))
            return False

    if username != None and password != None:
        cmd = "%s commit \"%s\" -m  \"commit by svn.py\"  --username '%s' --password '%s'" % (svn_bin(), svnpath, username, password)
    else:
        cmd = "%s commit \"%s\" -m  \"commit by svn.py\" " % (svn_bin(), svnpath)
    print(cmd)
    lines = os.popen(cmd).readlines()
    return True
