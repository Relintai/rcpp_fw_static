#include "http_server.h"

#include <functional>

#include "application.h"
#include "request.h"

#define LOG_VERBOSE 0

using namespace std::placeholders;

void HTTPServer::http_callback_handler(Request *request) {
	Application::handle_request(request);
}

void HTTPServer::httpEnterCallbackDefault(const brynet::net::http::HTTPParser &httpParser, const brynet::net::http::HttpSession::Ptr &session) {
	Request *request = RequestPool::get_request();

	request->http_parser = &httpParser;
	request->session = &session;

	request->setup_url_stack();

#if LOG_VERBOSE
	std::cout << "method:" << http_method_str(static_cast<http_method>(httpParser.method())) << std::endl;
#endif

	http_callback_handler(request);
}

void HTTPServer::wsEnterCallbackDefault(const brynet::net::http::HttpSession::Ptr &httpSession, brynet::net::http::WebSocketFormat::WebSocketFrameType opcode, const std::string &payload) {

	std::cout << "frame enter of type:" << int(opcode) << std::endl;
	std::cout << "payload is:" << payload << std::endl;
	// echo frame
	//auto frame = std::make_shared<std::string>();

	//WebSocketFormat::wsFrameBuild(payload.c_str(), payload.size(), *frame, WebSocketFormat::WebSocketFrameType::TEXT_FRAME, true, false);

	//httpSession->send(frame);
}

void HTTPServer::configure() {
}

void HTTPServer::initialize() {
	if (service)
		return;

	configure();

	service = brynet::net::TcpService::Create();
	service->startWorkerThread(threads);

	int p_port = port;

	//!
	if (listenBuilder)
		delete listenBuilder;

	listenBuilder = new brynet::net::wrapper::HttpListenerBuilder();
	listenBuilder->configureService(service);

	listenBuilder->configureSocketOptions({
			[](brynet::net::TcpSocket &socket) {
				socket.setNodelay();
			},
	});

	listenBuilder->configureConnectionOptions({ brynet::net::AddSocketOption::WithMaxRecvBufferSize(1024) });

	listenBuilder->configureListen([p_port](brynet::net::wrapper::BuildListenConfig builder) {
		builder.setAddr(false, "0.0.0.0", p_port);
	});

	listenBuilder->configureEnterCallback([](const brynet::net::http::HttpSession::Ptr &httpSession, brynet::net::http::HttpSessionHandlers &handlers) {
		handlers.setHttpCallback(HTTPServer::httpEnterCallbackDefault);
		handlers.setWSCallback(HTTPServer::wsEnterCallbackDefault);
	});

	listenBuilder->asyncRun();
}

void HTTPServer::main_loop() {
	while (true) {
		std::this_thread::sleep_for(std::chrono::seconds(1));

		if (brynet::base::app_kbhit()) {
			break;
		}
	}
}

HTTPServer::HTTPServer() {
	port = 80;
	threads = 4;
	listenBuilder = nullptr;
}

HTTPServer::~HTTPServer() {
	delete listenBuilder;
}
