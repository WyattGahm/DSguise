const char intro[] =
"DSguise - Wyatt Gahm\n"
"\n";

const char help[] = 
"Usage:\n"
"  dsguise <source.nds> <recipient.nds> -( T | I | N ) [-v]\n"
"  dsguise <source.nds> <recipient.nds> -( T | I | N )\n"
"  dsguise -?\n"
"\n"
"Options:\n"
"  -v   Output more information\n"
"  -?   Bring up this menu\n"
"  T    Copy the title\n"
"  I    Copy the icon\n"
"  N    Copy the name\n"
"\n"
"Examples:\n"
"  dsguise a.nds b.nds -TN\n"
"  dsguise -?\n"
"  dsguise a.nds b.nds -NI -v\n"
"  dsguise a.nds b.nds -NTI\n";

const char error[] = 
"Not enough information. Try -?\n";

