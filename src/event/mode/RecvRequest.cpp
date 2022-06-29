#include "RecvRequest.hpp"

#include <cerrno>
#include <iostream>
#include <map>
#include <unistd.h>

#include "CGI.hpp"
#include "Delete.hpp"
#include "EventRegister.hpp"
#include "Get.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "LocationConfig.hpp"
#include "Post.hpp"
#include "ReadCGI.hpp"
#include "ReadFile.hpp"
#include "SendResponse.hpp"
#include "ServerConfig.hpp"
#include "StreamSocket.hpp"
#include "URI.hpp"
#include "WriteCGI.hpp"

#include <iostream>
#include <utility>
#include <vector>

const size_t RecvRequest::BUF_SIZE = 2048;

RecvRequest::RecvRequest()
    : IOEvent(RECV_REQUEST), stream_(StreamSocket()), req_(HTTPRequest()),
      state_(HTTPParser::State(req_)) {}

RecvRequest::RecvRequest(StreamSocket stream)
    : IOEvent(stream.GetSocketFd(), RECV_REQUEST), stream_(stream),
      req_(HTTPRequest()), state_(HTTPParser::State(req_)) {}

RecvRequest::~RecvRequest() {}

void RecvRequest::Run() {
    char buf[BUF_SIZE];
    int  recv_size = recv(stream_.GetSocketFd(), buf, BUF_SIZE, 0);

    try {
        HTTPParser::update_state(state_, std::string(buf, recv_size));
    } catch (status::code code) {
        throw std::make_pair(stream_, code);
        // EventExecutor::onEventでcatch
    }
}

void RecvRequest::Register() { EventRegister::Instance().AddReadEvent(this); }

void RecvRequest::Unregister() { EventRegister::Instance().DelReadEvent(this); }

IOEvent* RecvRequest::RegisterNext() {
    if (state_.Phase() != HTTPParser::DONE) {
        return this;
    }

    // methodによって次のイベントが分岐
    try {
        this->Unregister();
        IOEvent* new_event = PrepareResponse(req_, stream_);
        return new_event;
    } catch (status::code code) { throw std::make_pair(stream_, code); }
    return NULL;
}

IOEvent* RecvRequest::PrepareResponse(const HTTPRequest&  req,
                                      const StreamSocket& stream) {
    IOEvent* new_event = NULL;

    // URI クラス作成
    URI uri(SearchServerConfig(req, stream), req.GetRequestTarget());
    uri.Init();

    HTTPParser::validate_request(uri, req);

    if (req.GetMethod() == "GET") {
        // Uriのパスや拡張子によって ReadFile or ReadCGI
        if (CGI::IsCGI(uri, "GET")) {
            class CGI cgi(uri, req);
            cgi.Run();
            new_event = new ReadCGI(cgi.FdForReadFromCGI(), stream, req);
        } else {
            Get get(stream, uri);
            get.Run();
            new_event = get.NextEvent();
        }

        // this->Unregister();
        new_event->Register();
        return new_event;
    } else if (req.GetMethod() == "POST") {
        if (CGI::IsCGI(uri, "POST")) {
            class CGI cgi(uri, req);
            cgi.Run();
            new_event = new WriteCGI(cgi, stream, req);
            // this->Unregister();
            new_event->Register();
            return new_event;
        } else {
            Post post(stream, req, uri);

            post.Run();
            // this->Unregister();
            return post.RegisterNext();
        }
    }
    if (req.GetMethod() == "DELETE") {
        Delete dlt(stream, uri);

        dlt.Run();
        // this->Unregister();
        return dlt.RegisterNext();
    }
    return NULL;
}

const ServerConfig RecvRequest::SearchServerConfig(const HTTPRequest&  req,
                                                   const StreamSocket& stream) {
    typedef std::vector<const ServerConfig>::const_iterator const_iterator;

    const std::vector<const ServerConfig> config_list =
        stream.GetServerConfig();

    const_iterator       it     = config_list.begin();
    const const_iterator it_end = config_list.end();

    for (; it != it_end; it++) {
        if (it->GetServerName() == req.GetHeaderValue("host")) {
            return *it;
        }
    }
    return *config_list.begin();
}
