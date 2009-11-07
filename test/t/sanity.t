# vi:filetype=perl

use lib 'lib';
use Test::Nginx::LWP;

plan tests => 2 * blocks();

#no_diff;

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


=== TEST 5: on & POST & read body
--- config
    chunkin on;
    location /main {
        echo "body:";
        echo $echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
["hello", "world" x 3024]
--- error_code: 200
--- response_body
body:
hello
--- SKIP


=== TEST 5: check the new headers filtered by chunkin
--- config
    chunkin on;
    location /headers {
        proxy_pass $scheme://127.0.0.1:$server_port/foo;
    }
    location /foo {
        echo $echo_client_request_headers;
    }
--- request
POST /headers
--- chunked_body eval
["hello", "world"]
--- error_code: 200
--- response_body eval
"POST /foo HTTP/1.0\r
Host: 127.0.0.1:\$ServerPort\r
Connection: close\r
User-Agent: Test::Nginx::LWP\r
Content-Type: text/plain\r
Transfer-Encoding: \r
Content-Length: 0\r
"

