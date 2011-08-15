# vi:filetype=

use lib 't/lib';
use Test::Nginx::Socket::Chunkin;

repeat_each(3);
#warn 'repeat each: ', repeat_each, "\n";

plan tests => repeat_each() * 2 * blocks();

no_diff;

run_tests();

__DATA__

=== TEST 1: binary in data
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
1b9\r
".'%Q`3ï¿½BBT0123456789AF%Q_ï¿½k%Q_ï¿½kwtk-emulatorSunMicrosystems_wtk AX7`encoding=pcm encoding=pcm&rate=8000&bits=8&channels=1 encoding=pcm&rate=22050&bits=16&chanZach-Laptop-W1.01.0en-US1.01.11.11.0SunMicrosystems_wtMIDP-2.11.0.10H,1Haudio/x-wavtruetruetruetruenencoding=rgb565&width=160&height=120 encoding=rgb565&width=320&height=240 encoding=rgb565&width=120&height=160encoding=jpeg encoding=png+12345678900http://mmsc.127.0.0.01'."\r
0\r
\r
"
--- response_body eval
'%Q`3ï¿½BBT0123456789AF%Q_ï¿½k%Q_ï¿½kwtk-emulatorSunMicrosystems_wtk AX7`encoding=pcm encoding=pcm&rate=8000&bits=8&channels=1 encoding=pcm&rate=22050&bits=16&chanZach-Laptop-W1.01.0en-US1.01.11.11.0SunMicrosystems_wtMIDP-2.11.0.10H,1Haudio/x-wavtruetruetruetruenencoding=rgb565&width=160&height=120 encoding=rgb565&width=320&height=240 encoding=rgb565&width=120&height=160encoding=jpeg encoding=png+12345678900http://mmsc.127.0.0.01'



=== TEST 2: CRLF in data (missing trailing CRLF)
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
2\r
\r
0\r
\r
"
--- response_body_like: 400 Bad Request
--- error_code: 400



=== TEST 3: CRLF in data
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
2\r
\r
\r
0\r
\r
"
--- response_body eval
"\r\n"



=== TEST 4: leading CRLF in data
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
4\r
\r
ab\r
0\r
\r
"
--- response_body eval
"\r\nab"



=== TEST 5: trailing CRLF in data
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
4\r
ab\r
\r
0\r
\r
"
--- response_body eval
"ab\r\n"



=== TEST 6: embedded CRLF in data
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
6\r
ab\r
cd\r
0\r
\r
"
--- response_body eval
"ab\r\ncd"



=== TEST 7: 413 in proxy
--- config
    chunkin on;
    location /main {
        proxy_pass $scheme://127.0.0.1:$server_port/proxy;
    }
    location /proxy {
        return 411;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /main
2\r
ab\r
0\r
\r
"
--- response_body_like: 411 Length Required
--- error_code: 411



=== TEST 8: padding spaces
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
6  \r
ab\r
cd\r
0\r
\r
"
--- response_body eval
"ab\r\ncd"



=== TEST 9: padding spaces (using HT)
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
6  \t\r
ab\r
cd\r
0\r
\r
"
--- response_body eval
"ab\r\ncd"



=== TEST 10: padding spaces (using \t and ' ' in last chunk)
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
6  \t\r
ab\r
cd\r
0\t \r
\r
"
--- response_body eval
"ab\r\ncd"



=== TEST 11: padding LWS (using \t and ' '  with a leading CRLF in last chunk)
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
6\r
\r
\t\r
ab\r
cd\r
0\t\r
\r
\r
\r
"
--- response_body_like: 400 Bad Request
--- error_code: 400



=== TEST 12: leading CRLF
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
\r
6\r
ab\r
cd\r
0\r
\r
"
--- response_body_like: 400 Bad Request
--- error_code: 400



=== TEST 13: zero-padding
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
0006\r
ab\r
cd\r
0\r
\r
"
--- response_body eval
"ab\r\ncd"



=== TEST 14: leading new lines
--- config
    chunkin on;
    location /ar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
\n \11\r
0006\r
ab\r
cd\r
0\r
\r
"
--- response_body_like: 400 Bad Request
--- error_code: 400



=== TEST 15: internal guard
--- config
    chunkin on;
    location /ar.do {
        internal;
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
\n \11\r
0006\r
ab\r
cd\r
0\r
\r
"
--- response_body_like: 404 Not Found
--- error_code: 404



=== TEST 16: phase issue
--- config
    chunkin on;
    location /ar.do {
        deny all;
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"POST /ar.do
1\r
a\r
0\r
\r
"
--- response_body_like: 403 Forbidden
--- error_code: 403



=== TEST 17: contenth-length AND chunked
--- config
    chunkin on;
    location /aar.do {
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- raw_request eval
"POST /aar.do HTTP/1.1\r
Host: data.test.com\r
Content-Type:application/octet-stream\r
transfer-encoding:chunked\r
User-Agent: SEC-SGHD840/1.0 NetFront/3.2 Profile/MIDP-2.0 Configuration/CLDC-1.1 UNTRUSTED/1.0\r
Content-Length: 6263\r
Via: ZXWAP GateWay,ZTE Technologies\r
x-up-calling-line-id: 841223657459\r
Connection: close\r
\r
5\r
hello\r
0\r
\r
"
--- response_body: hello
--- timeout: 1



=== TEST 18: Content-length AND chunked
--- config
    chunkin on;
    location /aar.do {
        echo_request_body;
    }
--- raw_request eval
["POST /aar.do HTTP/1.1\r
Host: data.test.com\r
Content-Type:application/octet-stream\r
transfer-encoding:chunked\r
User-Agent: SEC-SGHD840/1.0 NetFront/3.2 Profile/MIDP-2.0 Configuration/CLDC-1.1 UNTRUSTED/1.0\r
Content-Length: 6263\r
Via: ZXWAP GateWay,ZTE Technologies\r
x-up-calling-line-id: 841223657459\r
Connection: close\r
\r
5\r
", "hell","o\r
", "0\r
\r
"]
--- raw_request_middle_delay: 0.001
--- response_body: hello
--- timeout: 4



=== TEST 19: Content-length AND chunked (ready for the read_discard_request_body to work)
--- config
    chunkin on;
    location /aar.do {
        echo_request_body;
    }
--- raw_request eval
["POST /aar.do HTTP/1.1\r
Host: data.test.com\r
Content-Type:application/octet-stream\r
transfer-encoding:chunked\r
User-Agent: SEC-SGHD840/1.0 NetFront/3.2 Profile/MIDP-2.0 Configuration/CLDC-1.1 UNTRUSTED/1.0\r
Content-Length: 6263\r
Via: ZXWAP GateWay,ZTE Technologies\r
x-up-calling-line-id: 841223657459\r
Connection: close\r
\r
5\r
", "hell","o\r
", "0\r
\r
"]
--- raw_request_middle_delay: 0
--- response_body: hello
--- timeout: 1



=== TEST 20: packets in a single buf
--- config
    chunkin on;
    location /aar.do {
        client_body_buffer_size 1m;
        echo_request_body;
    }
--- raw_request eval
["POST /aar.do HTTP/1.1\r
Content-Type: application/octet-stream\r
Connection: close\r
Host: data.test.com\r
Transfer-Encoding: chunked\r
User-Agent: SonyEricssonW395/R1BA010 Profile/MIDP-2.1 Configuration/CLDC-1.1 UNTRUSTED/1.0\r
\r
",
"535\r
".('a' x 1228),
('a' x 107), "\r
", "0\r
\r
"]
--- raw_request_middle_delay: 0.002
--- response_body: hello
--- timeout: 2
--- SKIP



=== TEST 21: packets in a single buf
--- config
    chunkin on;
    location /aar.do {
        client_body_buffer_size 1m;
        echo_request_body;
    }
--- raw_request eval
["POST /aar.do HTTP/1.1\r
Content-Type: application/octet-stream\r
Connection: close\r
Host: data.test.com\r
Transfer-Encoding: chunked\r
User-Agent: SonyEricssonW395/R1BA010 Profile/MIDP-2.1 Configuration/CLDC-1.1 UNTRUSTED/1.0\r
\r
",
"535\r
".('a' x 1228),
('a' x 105) . "\r
", "0\r
\r
"]
--- raw_request_middle_delay: 0.002
--- response_body eval
'a' x 0x535
--- timeout: 5



=== TEST 22: packets in a single buf
--- config
    chunkin on;
    location /aar.do {
        client_body_buffer_size 1m;
        echo_request_body;
    }
--- raw_request eval
["POST /aar.do HTTP/1.1\r
Content-Type: application/octet-stream\r
Connection: close\r
Host: data.test.com\r
Transfer-Encoding: chunked\r
User-Agent: SonyEricssonW395/R1BA010 Profile/MIDP-2.1 Configuration/CLDC-1.1 UNTRUSTED/1.0\r
\r
",
"7e0\r
".('a' x 2016)."\r",
"aaaa
", "0\r
\r
"]
--- raw_request_middle_delay: 0.002
--- response_body eval
'a' x 0x7e0
--- timeout: 2
--- SKIP



=== TEST 23: not exceeding max body limit (chunk spanning preread and rb->buf)
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    10m;
        client_max_body_size       10m;
        chunkin_max_chunks_per_buf 2048;

        echo_read_request_body;

        echo_request_body;
        echo;
    }
--- more_headers
#Transfer-Encoding: chunked
--- request eval
"POST /main
".("a" x (1 * 1024 * 1024))
--- response_body eval
"a" x (1 * 1024 * 1024)
--- timeout: 60
--- SKIP



=== TEST 24: binary in data
--- config
    chunkin on;
    location /ar.do {
        echo $request_method;
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"DELETE /ar.do
5\r
hello\r
0\r
\r
"
--- response_body chop
DELETE
hello



=== TEST 25: CR LF across the boundary
--- config
    chunkin on;
    location /ar.do {
        echo $request_method;
        echo_request_body;
    }
--- more_headers
Transfer-Encoding: chunked
--- request eval
"DELETE /ar.do
5\r
hell\r\r
0\r
\r
"
--- response_body eval
"DELETE
hell\r"

