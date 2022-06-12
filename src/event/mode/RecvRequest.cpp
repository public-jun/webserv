#include "RecvRequest.hpp"

#include <cerrno>
#include <iostream>
#include <unistd.h>

#include "CGI.hpp"
#include "EventRegister.hpp"
#include "HTTPResponse.hpp"
#include "HTTPStatus.hpp"
#include "ReadCGI.hpp"
#include "ReadFile.hpp"
#include "SendResponse.hpp"
#include "ServerConfig.hpp"
#include "StreamSocket.hpp"
#include "WriteCGI.hpp"
#include "URI.hpp"

#include <iostream>
#include <utility>

const size_t RecvRequest::buf_size = 2048;

RecvRequest::RecvRequest()
    : IOEvent(RECV_REQUEST), stream_(StreamSocket()), req_(HTTPRequest()),
      state_(HTTPParser::State(req_)) {}

RecvRequest::RecvRequest(StreamSocket stream)
    : IOEvent(stream.GetSocketFd(), RECV_REQUEST), stream_(stream),
      req_(HTTPRequest()), state_(HTTPParser::State(req_)) {}

RecvRequest::~RecvRequest() {}

void RecvRequest::Run() {
    char buf[buf_size];
    int  recv_size = recv(stream_.GetSocketFd(), buf, buf_size, 0);

    try {
        HTTPParser::update_state(state_, std::string(buf, recv_size));
    } catch (status::code code) {
        // parseエラーが起きた場合
        // TODO: pairを投げる関数作る
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
    IOEvent* new_event = prepareResponse();

    return new_event;
}

IOEvent* RecvRequest::prepareResponse() {
    IOEvent* new_event = NULL;

    // URI クラス作成
    URI uri(searchServerConfig(), req_.GetRequestTarget());

    if (req_.GetMethod() == "GET") {
        // Uriのパスや拡張子によって ReadFile or ReadCGI
        if (CGI::IsCGI(req_.GetRequestTarget())) {
            class CGI cgi(req_);
            cgi.Run();
            new_event = new ReadCGI(cgi.FdForReadFromCGI(), stream_, req_);
        } else {
            new_event = new ReadFile(stream_, req_);
        }

        this->Unregister();
        new_event->Register();
        return new_event;
    } else if (req_.GetMethod() == "POST") {
        if (CGI::IsCGI(req_.GetRequestTarget())) {
            class CGI cgi(req_);
            cgi.Run();
            new_event = new WriteCGI(cgi, stream_, req_);
            this->Unregister();
            new_event->Register();
            return new_event;
        }
    }
    return NULL;
}

ServerConfig RecvRequest::searchServerConfig() {
    typedef std::vector<const ServerConfig>::const_iterator const_iterator;

    const std::vector<const ServerConfig> config_list =
        stream_.GetServerConfig();

    ServerConfig config = *config_list.begin();

    const_iterator       it     = config_list.begin();
    const const_iterator it_end = config_list.end();

    for (; it != it_end; it++) {
        if (it->getServerName() == req_.GetHeaderValue("Host")) {
            config = *it;
        }
    }
    return config;
}