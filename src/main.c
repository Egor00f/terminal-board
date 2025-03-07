#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ksys.h>
#include <kolibri_libini.h>

const char* LogFile = "/tmp0/1/Board.log";
const char* SettingFile = "/tmp0/1/Board.ini";

char debug_readc()
{
    bool empty;
    char byte;

    asm_inline(
        "int $0x40"
        : "=a"(byte), "=b"(empty)
        : "a"(63), "b"(2)
    );


    if (empty)
        return byte;
    else
        return 0;
}

void debug_reads(char* s, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        s[i] = debug_readc();
        if (!s[i])
            break;
    }
}

inline char GetColorCode(const char* name)
{
    char code;

    if (strcmp(name, "Black"))
    {
        code = '0';
    }
    else if (strcmp(name, "Red"))
    {
        code = '1';
    }
    else if (strcmp(name, "Green"))
    {
        code = '2';
    }
    else if (strcmp(name, "Brown"))
    {
        code = '3';
    }
    else if (strcmp(name, "Blue"))
    {
        code = '4';
    }
    else if (strcmp(name, "Purple"))
    {
        code = '5';
    }
    else if (strcmp(name, "Tortoise"))
    {
        code = '6';
    }
    else if (strcmp(name, "White"))
    {
        code = '7';
    }

    return code;
}

char line[4096];    // current line
size_t i = 0;
FILE* f;

char* Kernel;
char* Normal;

void LoadSetting()
{
    printf("LoadSettings\n");

    char NormalChars = GetColorCode(LIBINI_get_str(SettingFile, "Normal", "Chars", 16, "White"));
    char NormalBackground = GetColorCode(LIBINI_get_str(SettingFile, "Normal", "Background", 16, "Black"));

    char KernelChars = GetColorCode(LIBINI_get_str(SettingFile, "Kernel", "Chars", 16, "Purple"));
    char KernelBackground = GetColorCode(LIBINI_get_str(SettingFile, "Kernel", "Background", 16, "Black"));

    sprintf(Kernel, "\033[%c;%cm", KernelChars, KernelBackground);
    sprintf(Normal, "\033[%c;%cm", NormalChars, NormalBackground);

    fputs(Normal, stdout);
}

void print()
{
    if (memcmp(line, "K : ", 4))
    {
        fputs(Kernel, stdout);
    }

    line[i + 1] = '\0';
    printf(line);
    fprintf(f, line);

    fputs(Normal, stdout);
    memset(line, '\0', i);
    i = 0;
}

int main()
{
    bool nothing = false;

    memset(line, 0, 4096);

    if (!access(LogFile, F_OK))
        _ksys_file_create(LogFile);

    if (!access(SettingFile, F_OK))
    {
        _ksys_file_create(SettingFile);

        printf("Create settings file\n");

        LIBINI_set_str(SettingFile, "Normal", "Chars", 16, "White");
        LIBINI_set_str(SettingFile, "Normal", "Background", 16, "Black");
        LIBINI_set_str(SettingFile, "Kernel", "Chars", 16, "Purple");
        LIBINI_set_str(SettingFile, "Kernel", "Background", 16, "White");
    }

    LoadSetting();

    f = fopen(LogFile, "w");

    while (true)
    {
        char byte = debug_readc();
        if (byte)
        {
            line[i] = byte;
            i++;

            if (byte == '\n')
            {
                print();
            }
        }
        else if (nothing)
        {
            print();
        }
        else
        {
            nothing = true;
            _ksys_delay(10);
        }

        _ksys_thread_yield();
    }

    fclose(f);

    return 0;
}
