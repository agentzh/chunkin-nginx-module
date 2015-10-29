Name
====

**ngx_chunkin** - HTTP 1.1 chunked-encoding request body support for Nginx.

*This module is not distributed with the Nginx source.* See [the installation instructions](#installation).

Table of Contents
=================

* [Status](#status)
* [Version](#version)
* [Synopsis](#synopsis)
* [Description](#description)
    * [How it works](#how-it-works)
* [Directives](#directives)
    * [chunkin](#chunkin)
    * [chunkin_resume](#chunkin_resume)
    * [chunkin_max_chunks_per_buf](#chunkin_max_chunks_per_buf)
    * [chunkin_keepalive](#chunkin_keepalive)
* [Installation](#installation)
    * [Installation on Ubuntu 10.04 LTS using apt/dpkg](#installation-on-ubuntu-1004-lts-using-aptdpkg)
    * [For Developers](#for-developers)
* [Packages from users](#packages-from-users)
    * [Fedora 13 RPM files](#fedora-13-rpm-files)
* [Nginx Compatibility](#nginx-compatibility)
* [Community](#community)
    * [English Mailing List](#english-mailing-list)
    * [Chinese Mailing List](#chinese-mailing-list)
* [Bugs and Patches](#bugs-and-patches)
* [Source Repository](#source-repository)
* [ChangeLog](#changelog)
    * [v0.22](#v022)
    * [v0.21](#v021)
    * [v0.20](#v020)
    * [v0.19](#v019)
    * [v0.18](#v018)
    * [v0.17](#v017)
    * [v0.16](#v016)
    * [v0.15](#v015)
    * [v0.14](#v014)
    * [v0.11](#v011)
    * [v0.10](#v010)
    * [v0.08](#v008)
    * [v0.07](#v007)
    * [v0.06](#v006)
* [Test Suite](#test-suite)
* [Known Issues](#known-issues)
* [TODO](#todo)
* [Getting involved](#getting-involved)
* [Author](#author)
* [Copyright & License](#copyright--license)
* [See Also](#see-also)

Status
======

This module is no longer needed for Nginx 1.3.9+ because since 1.3.9, the Nginx core already has built-in support for the chunked request bodies.

And this module is now only maintained for Nginx versions older than 1.3.9.

Version
=======

This document describes ngx_chunkin [v0.23](http://github.com/agentzh/chunkin-nginx-module/tags) released on February 8, 2013.

Synopsis
========

```nginx

  chunkin on;
  
  error_page 411 = @my_411_error;
  location @my_411_error {
      chunkin_resume;
  }
  
  location /foo {
      # your fastcgi_pass/proxy_pass/set/if and
      # any other config directives go here...
  }
  ...
```

```nginx

  chunkin on;
  
  error_page 411 = @my_411_error;
  location @my_411_error {
      chunkin_resume;
  }
  
  location /bar {
    chunkin_keepalive on;  # WARNING: too experimental!
    
    # your fastcgi_pass/proxy_pass/set/if and
    # any other config directives go here...
  }
```

[Back to TOC](#table-of-contents)

Description
===========

This module adds [HTTP 1.1 chunked](http://tools.ietf.org/html/rfc2616#section-3.6.1) input support for Nginx without the need of patching the Nginx core.

Behind the scene, it registers an access-phase handler that will eagerly read and decode incoming request bodies when a `Transfer-Encoding: chunked` header triggers a `411` error page in Nginx. For requests that are not in the `chunked` transfer encoding, this module is a "no-op".

To enable the magic, just turn on the [chunkin](#chunkin) config option and define a custom `411 error_page` using [chunkin_resume](#chunkin_resume), like this:

```nginx

  server {
    chunkin on;

    error_page 411 = @my_411_error;
    location @my_411_error {
        chunkin_resume;
    }

    ...
  }
```

No other modification is required in your nginx.conf file and everything should work out of the box including the standard [proxy module](http://nginx.org/en/docs/http/ngx_http_proxy_module.html) (except for those [known issues](#known-issues)). Note that the [chunkin](#chunkin) directive is not allowed in the location block while the [chunkin_resume](#chunkin_resume) directive is only allowed on in `locations`.

The core module's [client_body_buffer_size](http://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_buffer_size), [client_max_body_size](http://nginx.org/en/docs/http/ngx_http_core_module.html#client_max_body_size), and [client_body_timeout](http://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_timeout) directive settings are honored. Note that, the "body sizes" here always indicate chunked-encoded body, not the data that has already been decoded. Basically, the
chunked-encoded body will always be slightly larger than the original data that is not encoded.

The [client_body_in_file_only](http://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_in_file_only) and [client_body_in_single_buffer](http://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_in_single_buffer) settings are followed partially. See [Know Issues](#known-issues).

This module is not supposed to be merged into the Nginx core because I've used [Ragel](http://www.complang.org/ragel/) to generate the chunked encoding parser for joy :)

[Back to TOC](#table-of-contents)

How it works
------------

Nginx explicitly checks chunked `Transfer-Encoding` headers and absent content length header in its very
early phase. Well, as early as the `ngx_http_process_request_header`
function. So this module takes a rather tricky approach. That is, use an output filter to intercept the `411 Length Required` error page response issued by `ngx_http_process_request_header`,
fix things and finally issue an internal redirect to the current location,
thus starting from those phases we all know and love, this time
bypassing the horrible `ngx_http_process_request_header` function.

In the `rewrite` phase of the newly created request, this module eagerly reads in the chunked request body in a way similar to that of the standard `ngx_http_read_client_request_body` function, but using its own chunked parser generated by Ragel. The decoded request body will be put into `r->request_body->bufs` and a corresponding `Content-Length` header will be inserted into `r->headers_in`.

Those modules using the standard `ngx_http_read_client_request_body` function to read the request body will just work out of box because `ngx_http_read_client_request_body` returns immediately when it sees `r->request_body->bufs` already exists.

Special efforts have been made to reduce data copying and dynamic memory allocation.

[Back to TOC](#table-of-contents)

Directives
==========

[Back to TOC](#table-of-contents)

chunkin
-------
**syntax:** *chunkin on|off*

**default:** *off*

**context:** *http, server*

**phase:** *access*

Enables or disables this module's hooks.

[Back to TOC](#table-of-contents)

chunkin_resume
--------------
**syntax:** *chunkin_resume*

**default:** *no*

**context:** *location*

**phase:** *content*

This directive must be used in your custom `411 error page` location to help this module work correctly. For example:

```nginx

  error_page 411 = @my_error;
  location @my_error {
      chunkin_resume;
  }
```

For the technical reason behind the necessity of this directive, please read the `nginx-devel` thread [Content-Length is not ignored for chunked requests: Nginx violates RFC 2616](http://nginx.org/pipermail/nginx-devel/2009-December/000041.html).

This directive was first introduced in the [v0.17](#v017) release.

[Back to TOC](#table-of-contents)

chunkin_max_chunks_per_buf
--------------------------
**syntax:** *chunkin_max_chunks_per_buf &lt;number&gt;*

**default:** *512*

**context:** *http, server, location*

Set the max chunk count threshold for the buffer determined by the [client_body_buffer_size](http://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_buffer_size) directive.
If the average chunk size is `1 KB` and your [client_body_buffer_size](http://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_buffer_size) setting
is 1 meta bytes, then you should set this threshold to `1024` or `2048`.

When the raw body size is exceeding [client_body_buffer_size](http://nginx.org/en/docs/http/ngx_http_core_module.html#client_body_buffer_size) *or* the chunk counter is exceeding this `chunkin_max_chunks_per_buf` setting, the decoded data will be temporarily buffered into disk files, and then the main buffer gets cleared and the chunk counter gets reset back to 0 (or `1` if there's a "pending chunk").

This directive was first introduced in the [v0.17](#v017) release.

[Back to TOC](#table-of-contents)

chunkin_keepalive
-----------------
**syntax:** *chunkin_keepalive on|off*

**default:** *off*

**context:** *http, server, location, if*

Turns on or turns off HTTP 1.1 keep-alive and HTTP 1.1 pipelining support.

Keep-alive without pipelining should be quite stable but pipelining support is very preliminary, limited, and almost untested.

This directive was first introduced in the [v0.07 release](#v007).

**Technical note on the HTTP 1.1 pipeling support**

The basic idea is to copy the bytes left by my chunked parser in
`r->request_body->buf` over into `r->header_in` so that nginx's
`ngx_http_set_keepalive` and `ngx_http_init_request` functions will pick
it up for the subsequent pipelined requests. When the request body is
small enough to be completely preread into the `r->header_in` buffer,
then no data copy is needed here -- just setting `r->header_in->pos`
correctly will suffice.

The only issue that remains is how to enlarge `r->header_in` when the
data left in `r->request_body->buf` is just too large to be hold in the
remaining room between `r->header_in->pos` and `r->header_in->end`. For
now, this module will just give up and simply turn off `r->keepalive`.

I know we can always use exactly the remaining room in `r->header_in` as
the buffer size when reading data from `c->recv`, but's suboptimal when
the remaining room in `r->header_in` happens to be very small while
`r->request_body->buf` is quite large.

I haven't fully grokked all the details among `r->header_in`, `c->buffer`,
busy/free lists and those so-called "large header buffers". Is there a
clean and safe way to reallocate or extend the `r->header_in` buffer?

[Back to TOC](#table-of-contents)

Installation
============

Grab the nginx source code from [nginx.org](http://nginx.org/), for example,
the version 1.2.6 (see [nginx compatibility](#compatibility)), and then build the source with this module:

```bash

wget 'http://nginx.org/download/nginx-1.2.6.tar.gz'
tar -xzvf nginx-1.2.6.tar.gz
cd nginx-1.2.6/

# Here we assume you would install you nginx under /opt/nginx/.
./configure --prefix=/opt/nginx \
    --add-module=/path/to/chunkin-nginx-module
 
make -j2
make install
```

Download the latest version of the release tarball of this module from [chunkin-nginx-module file list](http://github.com/agentzh/chunkin-nginx-module/tags).


[Back to TOC](#table-of-contents)

Installation on Ubuntu 10.04 LTS using apt/dpkg
-----------------------------------------------

You need to have dpkg-dev installed (apt-get install dpkg-dev)
And this guide assumes we are using the official repo.
```bash

sudo -s
apt-get source nginx
apt-get build-dep nginx
wget 'https://github.com/agentzh/chunkin-nginx-module/tarball/v0.23rc2'
tar -xzvf v0.23rc2

#rename directory to make it easier to remember later.
mv agentzh-chunkin-* chunkin

#this next one of course will change depending on which repo/version you are using.
cd nginx-1.0.14/
vim debian/rules

#See the ./configure section (for both "override_dh_auto_build": and "configure_debug:")
#At this point it's a good idea to have a idea of what you will need of modules/addons. remove any lines you don't need.
#The current last item in the ./configure section needs to have '\' added.
#Then add this: --add-module=../chunkin

dpkg-buildpackage

cd ..
#the next one of course will change according to version/build
dpkg -i nginx_1.0.14-1~lucid_i386.deb

#verify install with nginx -V see that add-module=../chunkin is in the configurea arguemnts list.

```

[Back to TOC](#table-of-contents)

For Developers
--------------

The chunked parser is generated by [Ragel](http://www.complang.org/ragel/). If you want to
regenerate the parser's C file, i.e., [src/chunked_parser.c](http://github.com/agentzh/chunkin-nginx-module/blob/master/src/chunked_parser.c), use
the following command from the root of the chunkin module's source tree:

```bash

$ ragel -G2 src/chunked_parser.rl
```

[Back to TOC](#table-of-contents)

Packages from users
===================

[Back to TOC](#table-of-contents)

Fedora 13 RPM files
-------------------

The following source and binary rpm files are contributed by Ernest Folch, with nginx 0.8.54, ngx_chunkin v0.21 and ngx_headers_more v0.13:

* [nginx-0.8.54-1.fc13.src.rpm](http://agentzh.org/misc/nginx/ernest/nginx-0.8.54-1.fc13.src.rpm)
* [nginx-0.8.54-1.fc13.x86_64.rpm](http://agentzh.org/misc/nginx/ernest/nginx-0.8.54-1.fc13.x86_64.rpm)

[Back to TOC](#table-of-contents)

Nginx Compatibility
===================

The following versions of Nginx should work with this module:

* **1.2.x**                       (last tested: 1.2.6)
* **1.1.x**                       (last tested: 1.1.5)
* **1.0.x**                       (last tested: 1.0.10)
* **0.8.x**                       (last tested: 0.8.54)
* **0.7.x >= 0.7.21**             (last tested: 0.7.67)

Earlier versions of Nginx like 0.6.x and 0.5.x will *not* work.

If you find that any particular version of Nginx above 0.7.21 does not work with this module, please consider [reporting a bug](#report-bugs).

[Back to TOC](#table-of-contents)

Community
=========

[Back to TOC](#table-of-contents)

English Mailing List
--------------------

The [openresty-en](https://groups.google.com/group/openresty-en) mailing list is for English speakers.

[Back to TOC](#table-of-contents)

Chinese Mailing List
--------------------

The [openresty](https://groups.google.com/group/openresty) mailing list is for Chinese speakers.

[Back to TOC](#table-of-contents)

Bugs and Patches
================

Please submit bug reports, wishlists, or patches by

1. creating a ticket on the [GitHub Issue Tracker](http://github.com/agentzh/chunkin-nginx-module/issues),
1. or posting to the [OpenResty community](#community).

[Back to TOC](#table-of-contents)

Source Repository
=================

Available on github at [agentzh/chunkin-nginx-module](http://github.com/agentzh/chunkin-nginx-module).

[Back to TOC](#table-of-contents)

ChangeLog
=========

[Back to TOC](#table-of-contents)

v0.22
-----
* now we remove the request header Transfer-Encoding completely because at least Apache will complain about the empty-value `Transfer-Encoding` request header. thanks hoodoos and Sandesh Kotwal.
* now we allow DELETE requests with chunked request bodies per hoodoos's request.
* now we use the 2-clause BSD license.

[Back to TOC](#table-of-contents)

v0.21
-----
* applied a patch from Gong Kaihui (龚开晖) to always call `post_handler` in `ngx_http_chunkin_read_chunked_request_body`.

[Back to TOC](#table-of-contents)

v0.20
-----
* fixed a bug that may read incomplete chunked body. thanks Gong Kaihui (龚开晖).
* fixed various memory issues in the implementation which may cause nginx processes to crash.
* added support for chunked PUT requests.
* now we always require "error_page 411 @resume" and no default (buggy) magic any more. thanks Gong Kaihui (龚开晖).

[Back to TOC](#table-of-contents)

v0.19
-----
* we now use ragel -G2 to generate the chunked parser and we're 36% faster.
* we now eagerly read the data octets in the chunked parser and we're 43% faster.

[Back to TOC](#table-of-contents)

v0.18
-----
* added support for `chunk-extension` to the chunked parser as per [RFC 2616](http://tools.ietf.org/html/rfc2616#section-3.6.1), but we just ignore them (if any) because we don't understand them.
* added more diagnostic information for certian error messages.

[Back to TOC](#table-of-contents)

v0.17
-----
* implemented the [chunkin_max_chunks_per_buf](#chunkin_max_chunks_per_buf) directive to allow overriding the default `512` setting.
* we now bypass nginx's [discard requesty body bug](http://nginx.org/pipermail/nginx-devel/2009-December/000041.html) by requiring our users to define explicit `411 error_page` with [chunkin_resume](#chunkin_resume) in the error page location. Thanks J for reporting related bugs.
* fixed `r->phase_handler` in our post read handler. our handler may run one more time before :P
* the chunkin handler now returns `NGX_DECLINED` rather than `NGX_OK` when our `ngx_http_chunkin_read_chunked_request_body` function returns `NGX_OK`, to avoid bypassing other access-phase handlers.

[Back to TOC](#table-of-contents)

v0.16
-----
* turned off ddebug in the previous release. thanks J for reporting it.

[Back to TOC](#table-of-contents)

v0.15
-----
* fixed a regression that ctx->chunks_count never incremented in earlier versions.

[Back to TOC](#table-of-contents)

v0.14
-----
* now we no longer skip those operations between the (interrupted) ngx_http_process_request_header and the server rewrite phase. this fixed the security issues regarding the [internal](http://nginx.org/en/docs/http/ngx_http_core_module.html#internal) directive as well as SSL sessions.
* try to ignore CR/LF/SP/HT at the begining of the chunked body.
* now we allow HT as padding spaces and ignore leading CRLFs.
* improved diagnostic info in the error.log messages when parsefail occurs.

[Back to TOC](#table-of-contents)

v0.11
-----
* added a random valid-chunked-request generator in t/random.t.
* fixed a new connection leak issue caught by t/random.t.

[Back to TOC](#table-of-contents)

v0.10
-----
* fixed a serious bug in the chunked parser grammer: there would be ambiguity when CRLF appears in the chunked data sections. Thanks J for reporting it.

[Back to TOC](#table-of-contents)

v0.08
-----
* fixed gcc compilation errors on x86_64, thanks J for reporting it.
* used the latest Ragel 6.6 to generate the `chunked_parser.c` file in the source tree.

[Back to TOC](#table-of-contents)

v0.07
-----

* marked the disgarded 411 error page's output chain bufs as consumed by setting `buf->pos = buf->last`. (See [this nginx-devel thread](http://nginx.org/pipermail/nginx-devel/2009-December/000025.html) for more details.)
* added the [chunkin_keepalive](#chunkin_keepalive) directive which can enable HTTP 1.1 keep-alive and HTTP 1.1 pipelining, and defaults to `off`.
* fixed the `alphtype` bug in the Ragel parser spec; which caused rejection of non-ascii octets in the chunked data. Thanks J for his bug report.
* added `Test::Nginx::Socket` to test our nginx module on the socket level. Thanks J for his bug report.
* rewrote the bufs recycling part and preread-buf-to-rb-buf transition part, also refactored the Ragel parser spec, thus eliminating lots of serious bugs.
* provided better diagnostics in the error log message for "bad chunked body" parsefails in the chunked parser. For example:


     2009/12/02 17:35:52 [error] 32244#0: *1 bad chunked body (offset 7, near "4^M
     hell <-- HERE o^M
     0^M
     ^M
     ", marked by " <-- HERE ").
     , client: 127.0.0.1, server: localhost, request: "POST /main
    HTTP/1.1", host: "localhost"


* added some code to let the chunked parser handle special 0-size chunks that are not the last chunk.
* fixed a connection leak bug regarding incorrect `r->main->count` reference counter handling for nginx 0.8.11+ (well, the `ngx_http_read_client_request_body` function in the nginx core also has this issue, I'll report it later.)

[Back to TOC](#table-of-contents)

v0.06
-----
* minor optimization: we won't traverse the output chain link if the chain count is not large enough.

[Back to TOC](#table-of-contents)

Test Suite
==========

This module comes with a Perl-driven test suite. The [test cases](http://github.com/agentzh/chunkin-nginx-module/tree/master/test/t/) are
[declarative](http://github.com/agentzh/chunkin-nginx-module/blob/master/test/t/sanity.t) too. Thanks to the [Test::Base](http://search.cpan.org/perldoc?Test::Base) module in the Perl world.

To run it on your side:

```bash

$ cd test
$ PATH=/path/to/your/nginx-with-chunkin-module:$PATH prove -r t
```

You need to terminate any Nginx processes before running the test suite if you have changed the Nginx server binary.

At the moment, [LWP::UserAgent](http://search.cpan.org/perldoc?LWP::UserAgent) is used by the [test scaffold](http://github.com/agentzh/chunkin-nginx-module/blob/master/test/lib/Test/Nginx/LWP.pm) for simplicity.

Because a single nginx server (by default, `localhost:1984`) is used across all the test scripts (`.t` files), it's meaningless to run the test suite in parallel by specifying `-jN` when invoking the `prove` utility.

Some parts of the test suite requires modules [proxy](http://nginx.org/en/docs/http/ngx_http_proxy_module.html) and [echo](http://github.com/openresty/echo-nginx-module) to be enabled as well when building Nginx.

[Back to TOC](#table-of-contents)

Known Issues
============

* May not work with certain 3rd party modules like the [upload module](http://www.grid.net.ru/nginx/upload.en.html) because it implements its own request body reading mechanism.
* "client_body_in_single_buffer on" may *not* be obeyed for short contents and fast network.
* "client_body_in_file_only on" may *not* be obeyed for short contents and fast network.
* HTTP 1.1 pipelining may not fully work yet.

[Back to TOC](#table-of-contents)

TODO
====

* make the chunkin handler run at the end of the `access phase` rather than beginning.
* add support for `trailers` as specified in the [RFC 2616](http://tools.ietf.org/html/rfc2616#section-3.6.1).
* fix the [known issues](#known-issues).

[Back to TOC](#table-of-contents)

Getting involved
================

You'll be very welcomed to submit patches to the [author](#author) or just ask for a commit bit to the [source repository](#source-repository) on GitHub.

[Back to TOC](#table-of-contents)

Author
======

Yichun "agentzh" Zhang (章亦春) *&lt;agentzh@gmail.com&gt;*, CloudFlare Inc.

This wiki page is also maintained by the author himself, and everybody is encouraged to improve this page as well.

[Back to TOC](#table-of-contents)

Copyright & License
===================

The basic client request body reading code is based on the `ngx_http_read_client_request_body` function and its utility functions in the Nginx 0.8.20 core. This part of code is copyrighted by Igor Sysoev.

Copyright (c) 2009-2013, Yichun Zhang (agentzh), CloudFlare Inc.

This module is licensed under the terms of the BSD license.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the Taobao Inc. nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

[Back to TOC](#table-of-contents)

See Also
========

* The original thread on the Nginx mailing list that inspires this module's development: ["'Content-Length' header for POSTs"](http://forum.nginx.org/read.php?2,4453,20543).
* The orginal announcement thread on the Nginx mailing list: ["The chunkin module: Experimental chunked input support for Nginx"](http://forum.nginx.org/read.php?2,22967).
* The original [blog post](http://agentzh.spaces.live.com/blog/cns!FF3A735632E41548!481.entry) about this module's initial development.
* The thread discussing chunked input support on the nginx-devel mailing list: ["Chunked request body and HTTP header parser"](http://nginx.org/pipermail/nginx-devel/2009-December/000021.html).
* The [echo module](http://github.com/openresty/echo-nginx-module) for Nginx module's automated testing.
* [RFC 2616 - Chunked Transfer Coding](http://tools.ietf.org/html/rfc2616#section-3.6.1).

