#include <stdio.h>
#include <string.h>

enum {
    BUFSIZE = 256
};

%% machine chunked;
%% write data;

int main (int argc, char **argv)
{
    int cs, res = 0;
    char *p, *pe, *eof;
    char buf[BUFSIZE];
    size_t len = 0;
    size_t bytes_read;
    eof = NULL;
    size_t data_bytes_read = 0, chunk_size = 0;
    int chunk_size_order = 0;

    %%{
        action bad_last_chunk {
            fprintf(stderr, "bad last chunk (offset %d).\n", p - buf);
        }

        action bad_chunk_data {
            fprintf(stderr, "bad chunk data "
                "(bytes already read %d, bytes expected: %d): "
                "offset %d.\n", data_bytes_read, chunk_size, p - buf);
        }

        action test_len {
            data_bytes_read < chunk_size
        }

        action read_data_byte {
            data_bytes_read++;
            fprintf(stderr, "data bytes read: %d (char: %c)\n",
                    data_bytes_read, *p);
        }

        action start_reading_size {
            data_bytes_read = 0;
            chunk_size = 0;
            chunk_size_order = 0;
        }

        action read_size {
            chunk_size <<= 4;
            chunk_size_order++;
            if (*p >= 'A' && *p <= 'F') {
                chunk_size |= 10 + *p - 'A';
            } else if (*p >= 'a' && *p <= 'f') {
                chunk_size |= 10 + *p - 'a';
            } else {
                chunk_size |= *p - '0';
            }
            printf("INFO: chunk size: %d\n", chunk_size);
        }

        action verify_data {
            if (data_bytes_read != chunk_size) {
                fprintf(stderr, "ERROR: chunk size not meet: "
                    "%d != %d\n", data_bytes_read, chunk_size);
                fbreak;
            }
        }

        CRLF = "\r\n";

        chunk_size = (xdigit+ - "0") >start_reading_size $read_size;

        chunk_data_octet = any
                when test_len $err(bad_chunk_data) $read_data_byte;

        chunk_data = chunk_data_octet* %verify_data;

        chunk = chunk_size " "* CRLF
                        chunk_data CRLF;

        last_chunk = "0" " "* CRLF ${ fprintf(stderr, "in last chunk %d (cs: %d)\n", p - buf, cs); }; #$err(bad_last_chunk);

        main := (chunk*
                last_chunk
                CRLF) %err{ fprintf(stderr, "in end %d (cs: %d)\n", p - buf, cs); };

    }%%

    %% write init;

    while (len < BUFSIZE) {
        bytes_read =
            fread(buf + len, sizeof(char), BUFSIZE - len, stdin);
        if (bytes_read == 0) {
            break;
        }
        len += bytes_read;
        if (feof(stdin) || ferror(stdin)) {
            break;
        }
    }

    p = buf;
    pe = buf + len;

    %% write exec;

    printf("cs >= first_final: %d, execute = %i, p moved %d, "
            "p remaining: %d\n", cs >= chunked_first_final, res, p - buf, pe - p);

    return 0;
}

