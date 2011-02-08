# vi:filetype=

use lib 't/lib';
use Test::Nginx::LWP::Chunkin;

plan tests => repeat_each() * 2 * blocks();

#no_diff;

run_tests();

__DATA__

=== TEST 1: many little chunks
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    4;
        echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
[split //,
  "hello world blah blah blah! oh, yay! end"]
--- response_body eval
"hello world blah blah blah! oh, yay! end"



=== TEST 2: many little chunks (more!)
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    1k;
        #echo_sleep 500;
        echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
[split //,
  ("hello world blah blah blah! o, yah!" x 4) . 'end']
--- response_body eval
("hello world blah blah blah! o, yah!" x 4) . 'end'



=== TEST 3: many little chunks (more!)
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    60;
        #echo_sleep 500;
        echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
[split //,
  ("hello world blah blah blah! oh, yah!" x 100) . 'end']
--- response_body eval
("hello world blah blah blah! oh, yah!" x 100) . 'end'



=== TEST 4: exceeding max body limit (this test may fail randomly with the error "500 write failed: Connection reset by peer", which is considered OK).
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    512;
        client_max_body_size       1024;

        echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
[split //,
  ("a" x 1024) . 'e']
--- response_body_like: 413 Request Entity Too Large
--- error_code: 413



=== TEST 5: not exceeding max body limit (chunk spanning preread and rb->buf)
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    512;
        client_max_body_size       1048;

        echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
["a" x 1024]
--- response_body eval
"a" x 1024



=== TEST 6: next chunk reset bug
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    600;
        client_max_body_size       8k;

        echo_request_body;
    }
--- request
POST /main
--- middle_chunk_delay: 0.001
--- chunked_body eval
[split //,
  ("a" x 700) . 'e']
--- response_body eval
"a" x 700 . 'e'



=== TEST 7: next chunk reset bug (too many chunks)
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    8k;
        client_max_body_size       8k;

        echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
[split //,
  ("a" x 700) . 'e']
--- response_body eval
"a" x 700 . 'e'



=== TEST 8: normal POST with chunkin on
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    600;
        client_max_body_size       8k;

        echo_read_request_body;
        echo_request_body;
    }
--- request
POST /main
hello, world
--- response_body chomp
hello, world



=== TEST 9: not exceeding max body limit (chunk spanning preread and rb->buf)
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    10m;
        client_max_body_size       10m;

        echo_request_body;
        echo;
    }
--- request
POST /main
--- chunked_body eval
[split //, "a" x (500 * 1024)]
--- middle_chunk_delay: 0
--- response_body eval
"a" x (500 * 1024)
--- quit
--- SKIP

