#ifndef HTTPSTATUS_HPP
#define HTTPSTATUS_HPP

namespace status {

typedef int code;

const code ok                       = 200;
const code created                  = 201;
const code no_content               = 204;
const code bad_request              = 400;
const code forbidden                = 403;
const code not_found                = 404;
const code method_not_allowed       = 405;
const code request_entity_too_large = 413;
const code uri_too_long             = 414;
const code unsupported_media_type   = 415;
const code server_error             = 500;
const code version_not_suppoted     = 505;
} // namespace status

#endif // HTTPSTATUS_HPP
