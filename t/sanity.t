# vi:ft=

use lib 't/lib';
use Test::Nginx::LWP;

plan tests => repeat_each() * 2 * blocks();

no_diff;

run_tests();

__DATA__

=== TEST 1: on & GET
--- config
    location /main {
        echo hi;
    }
--- request
GET /main
--- response_body
hi
--- error_code: 200



=== TEST 2: on & POST
--- config
    location /main {
        echo_read_request_body;
        echo $request_method;
        echo $echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body
POST
helloworld



=== TEST 3: raw request headers (indeed chunked)
--- config
    location /main {
        echo_read_request_body;
        echo 'headers:';
        echo $echo_client_request_headers;
        echo_request_body;
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

helloworld"



=== TEST 4: request headers filtered by chunkin
This test passes only for nginx versions
* 0.7.x >= 0.7.21
* 0.8.x >= 0.8.10
--- config
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



=== TEST 5: 0 chunk body
--- config
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



=== TEST 6: 0 chunk body via proxy (header okay)
--- config
    location /main {
        proxy_pass $scheme://127.0.0.1:$server_port/proxy;
        #proxy_pass $scheme://127.0.0.1:1113/proxy;
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



=== TEST 7: single char in preread
--- config
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



=== TEST 8: single char in preread (headers okay)
--- config
    location /main {
        echo_read_request_body;
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



=== TEST 9: on & POST & read body & no single buf
--- config
    location /main {
        echo_read_request_body;
        echo "body:";
        echo_request_body;
    }
--- request
POST /main
--- middle_chunk_delay: 0.01
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body chop
body:
helloworld



=== TEST 10: on & POST & read body & single buf
--- config
    location /main {
        client_body_in_single_buffer on;
        echo_read_request_body;
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



=== TEST 11: on & POST & read body & no single buf & echo_request_body
--- config
    location /main {
        client_body_in_single_buffer on;
        echo "body:";
        echo_read_request_body;
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



=== TEST 12: request headers filtered by chunkin (with delay)
This test passes only for nginx versions
* 0.7.x >= 0.7.21
* 0.8.x >= 0.8.10
--- middle_chunk_delay: 0.01
--- config
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



=== TEST 13: small buf (using 2-byte buf)
--- config
    location /main {
        client_body_buffer_size    2;
        echo_read_request_body;
        echo "body:";
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



=== TEST 14: small buf (using 1-byte buf)
--- config
    location /main {
        client_body_buffer_size    1;
        echo "body:";
        echo_read_request_body;
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



=== TEST 15: small buf (using 3-byte buf)
--- config
    location /main {
        client_body_buffer_size    3;
        echo_read_request_body;
        echo "body:";
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



=== TEST 16: big chunk
--- config
    location /main {
        client_body_buffer_size    3;
        echo_read_request_body;
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



=== TEST 17: in memory
--- config
    location /main {
        client_body_buffer_size    4k;
        echo_read_request_body;
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



=== TEST 18: on & PUT
--- config
    location /main {
        echo_read_request_body;
        echo_request_body;
    }
--- request
PUT /main
--- chunked_body eval
["hello", "world"]
--- response_body chomp
helloworld
--- error_code: 200

