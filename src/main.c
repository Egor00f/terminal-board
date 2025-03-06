#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ksys.h>

const char* LogFile = "/tmp0/1/Board.log";

extern uint32_t con_set_flags(uint32_t new_flags) __stdcall;

char debug_readc()
{
    bool empty;
    char byte;

    asm_inline(
        "int $0x40"
		: "=a"(byte), "=b"(empty)
        : "a"(63), "b"(2)
    );
   

    if(empty)
    {
        return byte;
    }
    else
    {
        return 0;
    }
}

void debug_reads(char *s, size_t len)
{
    for(size_t i = 0; i < len; i++)
    {
        s[i] = debug_readc();
        if (!s[i])
            break;
    }
}

int main()
{
    bool nothing = false;

    char line[4096];    // current line
    size_t i = 0;
    FILE *f;

	memset(line, 0, 4096);

    if(!access(LogFile, F_OK))
        _ksys_file_create(LogFile);

    f = fopen(LogFile, "w");

    while(true)
    {        
        char byte = debug_readc();
        if(byte)
        {
            line[i] = byte;
			i++;

            if(byte == '\n')
            {   
                if (memcmp(line, "K : ", 4))
                {
                    fputs("\033[35;40m", stdout);
                }

                line[i + 1] = '\0';
                i = 0;
                printf(line);
                fprintf(f, line);

                fputs("\033[7m", stdout);
                memset(line, 0, i);
            }
        }
        /*else if (nothing)
        {
            print();
        }*/
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
