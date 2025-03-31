
void start()
{
    char *vidmem = (char*)0xb8000;
    *vidmem = 'N';
}