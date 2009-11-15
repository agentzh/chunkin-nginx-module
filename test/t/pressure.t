# vi:filetype=perl

use lib 'lib';
use Test::Nginx::LWP;

plan tests => 2 * blocks();

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
        client_body_buffer_size    8k;
        #echo_sleep 500;
        echo_request_body;
    }
--- request
POST /main
--- chunked_body eval
[split //,
  ("hello world blah blah blah! oh, yay!" x 100) . 'end']
--- response_body eval
("hello world blah blah blah! oh, yay!" x 100) . 'end'



=== TEST 3: exceeding max body limit
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



=== TEST 4: not exceeding max body limit (chunk spanning preread and rb->buf)
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



=== TEST 5: next chunk reset bug
--- config
    chunkin on;
    location /main {
        client_body_buffer_size    600;
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

