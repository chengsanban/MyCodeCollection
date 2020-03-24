void* MyMemcopy(void* dest, const void* src, size_t n)
{
    char* dest1 = (char*)dest;
    const char* src1 = (const char*)src;
    for (; n > 0; *dest1++ = *src1++, --n);
    return dest;
}


char* MyStrcopy(char *dest,const char* src)
{
    assert(dest&&src);
    char *dest1;
    for (dest1 = dest; (*dest1++=*src++)!= '\0';);
    return dest;
}


int MyStrcmp(const char*s1,const char*s2)
{
    assert(s1&&s2);
    while (*s1++==*s2++)
    {
        if (*s1 == '\0')
            return 0;
    }
    return (*s1 < *s2 ? -1 : +1);
}


char* MyStrcat(char* dest, const char* src)
{
    assert(dest&&src);
    char *dest1;
    for (dest1 = dest; *dest1 != '\0'; ++dest1);
    for (; (*dest1 = *src) != '\0'; ++dest1, ++src);
    return dest;
}

size_t MyStrlen(const char *s)
{
    assert(s);
    const char* s1;
    for (s1 = s; *s1 != '\0'; ++s1);
    return s1 - s;
}