void __attribute ((constructor)) phook_init(void){
    get_process_info();
#ifdef DO_REINSTALL
    reinstall();
#endif
}

void __attribute ((destructor)) phook_dest(void){
#ifdef DO_REINSTALL
    reinstall();
#endif
}
