# vi:ft=

use lib 't/lib';
use Test::Nginx::LWP::Chunkin;

plan tests => repeat_each() * 2 * blocks();

no_diff;

run_tests();

__DATA__

=== TEST 1: off & POST
--- config
    chunkin off;
    location /main {
        echo hi;
    }
--- request
POST /main
--- chunked_body eval
["hello", "world"]
--- error_code: 411
--- response_body_like: 411 Length Required



=== TEST 2: default (off) & POST
--- config
    location /main {
    }
--- request
POST /main
--- chunked_body eval
["hello", "world"]
--- error_code: 411
--- response_body_like: 411 Length Required



=== TEST 3: off & GET
--- config
    chunkin off;
    location /main {
        echo hi;
    }
--- request
GET /main
--- response_body
hi
--- error_code: 200



=== TEST 4: on & GET
--- config
    chunkin on;
    location /main {
        echo hi;
    }
--- request
GET /main
--- response_body
hi
--- error_code: 200



=== TEST 5: on & POST
--- config
    chunkin on;
    location /main {
        echo $request_method;
        #echo $echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body
POST



=== TEST 6: raw request headers (indeed chunked)
--- config
    chunkin on;
    location /main {
        echo 'headers:';
        echo $echo_client_request_headers;
    }
--- request
POST /main
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body eval
"headers:
POST /main HTTP/1.1\r
Host: localhost:\$ServerPortForClient\r
User-Agent: Test::Nginx::LWP\r
Content-Type: text/plain\r
Transfer-Encoding: chunked\r

"



=== TEST 7: request headers filtered by chunkin
This test passes only for nginx versions
* 0.7.x >= 0.7.21
* 0.8.x >= 0.8.10
--- config
    chunkin on;
    location /main {
        proxy_pass_request_headers on;
        proxy_pass $scheme://127.0.0.1:$server_port/proxy;
    }

    location /proxy {
        echo $echo_client_request_headers;
    }
--- request
POST /main
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body eval
"POST /proxy HTTP/1.0\r
Host: 127.0.0.1:\$ServerPort\r
Connection: close\r
User-Agent: Test::Nginx::LWP\r
Content-Type: text/plain\r
Content-Length: 10\r

"



=== TEST 8: 0 chunk body
--- config
    chunkin on;
    location /main {
        echo "body:";
        echo $echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
[""]
--- error_code: 200
--- response_body eval
"body:

"



=== TEST 9: 0 chunk body via proxy (header okay)
--- config
    chunkin on;
    location /main {
        proxy_pass $scheme://127.0.0.1:$server_port/proxy;
    }
    location /proxy {
        echo $echo_client_request_headers;
    }
--- request
POST /main
--- chunked_body eval
[""]
--- error_code: 200
--- response_body eval
"POST /proxy HTTP/1.0\r
Host: 127.0.0.1:\$ServerPort\r
Connection: close\r
User-Agent: Test::Nginx::LWP\r
Content-Type: text/plain\r
Content-Length: 0\r

"



=== TEST 10: single char in preread
--- config
    chunkin on;
    location /main {
        proxy_pass $scheme://127.0.0.1:$server_port/proxy;
    }
    location /proxy {
        echo $echo_client_request_headers;
    }
--- request
POST /main
--- chunked_body eval
["a"]
--- error_code: 200
--- response_body eval
"POST /proxy HTTP/1.0\r
Host: 127.0.0.1:\$ServerPort\r
Connection: close\r
User-Agent: Test::Nginx::LWP\r
Content-Type: text/plain\r
Content-Length: 1\r

"



=== TEST 11: single char in preread (headers okay)
--- config
    chunkin on;
    location /main {
        echo "body:";
        echo $echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
["a"]
--- error_code: 200
--- response_body
body:
a



=== TEST 12: on & POST & read body & no single buf
--- config
    chunkin on;
    location /main {
        echo "body:";
        echo $echo_request_body;
    }
--- request
POST /main
--- middle_chunk_delay: 0.01
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body
body:
helloworld



=== TEST 13: on & POST & read body & single buf
--- config
    chunkin on;
    location /main {
        client_body_in_single_buffer on;
        echo "body:";
        echo $echo_request_body;
    }
--- request
POST /main
--- middle_chunk_delay: 0.01
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body
body:
helloworld
--- skip_nginx: 2: < 0.7.58



=== TEST 14: on & POST & read body & no single buf & echo_request_body
--- config
    chunkin on;
    location /main {
        client_body_in_single_buffer on;
        echo "body:";
        echo_request_body;
        echo;
    }
--- request
POST /main
--- middle_chunk_delay: 0.01
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body
body:
helloworld
--- skip_nginx: 2: < 0.7.58



=== TEST 15: request headers filtered by chunkin (with delay)
This test passes only for nginx versions
* 0.7.x >= 0.7.21
* 0.8.x >= 0.8.10
--- middle_chunk_delay: 0.01
--- config
    chunkin on;
    location /main {
        proxy_pass_request_headers on;
        proxy_pass $scheme://127.0.0.1:$server_port/proxy;
    }

    location /proxy {
        echo $echo_client_request_headers;
    }
--- request
POST /main
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body eval
"POST /proxy HTTP/1.0\r
Host: 127.0.0.1:\$ServerPort\r
Connection: close\r
User-Agent: Test::Nginx::LWP\r
Content-Type: text/plain\r
Content-Length: 10\r

"



=== TEST 16: small buf (using 2-byte buf)
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    2;
        echo "body:";
        echo $echo_request_body;
        echo_request_body;
    }
--- request
POST /main
--- start_chunk_delay: 0.01
--- middle_chunk_delay: 0.01
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body eval
"body:

helloworld"



=== TEST 17: small buf (using 1-byte buf)
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    1;
        echo "body:";
        echo $echo_request_body;
        echo_request_body;
    }
--- request
POST /main
--- start_chunk_delay: 0.01
--- middle_chunk_delay: 0.01
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body eval
"body:

helloworld"



=== TEST 18: small buf (using 3-byte buf)
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    3;
        echo "body:";
        echo $echo_request_body;
        echo_request_body;
    }
--- request
POST /main
--- start_chunk_delay: 0.01
--- middle_chunk_delay: 0.01
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body eval
"body:

helloworld"



=== TEST 19: big chunk
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    3;
        echo "body:";
        echo $echo_request_body;
        echo_request_body;
    }
--- request
POST /main
--- start_chunk_delay: 0.01
--- middle_chunk_delay: 0.01
--- chunked_body eval
["hello", "world" x 1024, "!" x 1024]
--- error_code: 200
--- response_body eval
"body:

hello" . ("world" x 1024) . ('!' x 1024)



=== TEST 20: in memory
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    4k;
        echo "body:";
        echo $echo_request_body;
    }
--- request
POST /main
--- start_chunk_delay: 0.01
--- middle_chunk_delay: 0.01
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body
body:
helloworld



=== TEST 21: on & PUT
--- config
    chunkin on;
    location /main {
        #echo_read_request_body;
        echo_request_body;
    }
--- request
PUT /main
--- chunked_body eval
["hello", "world"]
--- response_body chomp
helloworld
--- error_code: 200

