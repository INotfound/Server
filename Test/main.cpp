/*
 * @Author: INotFound
 * @Date: 2020-12-17 00:35:45
 * @LastEditTime: 2021-01-18 06:00:25
 */
#define PERFORMANCE 1
#define _WIN32_WINNT 0x0601
#include <regex>
#include "Magic/Core/Container.h"
#include "Magic/NetWork/Http/Uri.h"
#include "Magic/NetWork/Http/Http.h"

#include "Magic/NetWork/Http/HttpServer.h"
#include "Magic/Utilty/Logger.h"
#include "Magic/Utilty/TimingWheel.h"
#include "Magic/DataBase/ConnectionPool.h"
#include "Magic/Core/Adapter.h"
#include "Magic/Utilty/Trace.h"


///// http://127.0.0.1/

class A;
class B{
public:
    friend class A;
    B(A* self):inner(self){}
    A* operator->(){
        return inner;
    }

    A& operator*(){
        return *inner;
    }

private:
    A* inner;
};


class A{
public:
    ObjectWrapper<A> bb() {return ObjectWrapper<A>(this);};
};

using namespace Magic::NetWork::Http;
Safe<WebSocket> webSocket;
class ResourceServlet :public Magic::NetWork::Http::IHttpServlet{
    public:
        ResourceServlet(){
        }

        void websocket(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket,const Safe<Magic::NetWork::Http::HttpRequest>& request,const Safe<Magic::NetWork::Http::HttpResponse>& response){
            webSocket = httpSocket->upgradeWebSocket(request,response);
            webSocket->sendTextMessage("xxxxxx");
            webSocket->recvTextMessage([](const Safe<WebSocket>& socket,const std::string& msg){
                MAGIC_DEBUG() << msg;
                socket->sendTextMessage(msg);
            });
            webSocket->disconnectedCallBack([](const Safe<WebSocket>& socket){
                MAGIC_DEBUG() << "disconnected";
            });
        }

        void handle1(const Safe<Magic::NetWork::Http::HttpSocket>& httpSocket,const Safe<Magic::NetWork::Http::HttpRequest>& request,const Safe<Magic::NetWork::Http::HttpResponse>& response){
            response->setBody("Hello World")->setStatus(HttpStatus::OK);
            httpSocket->sendResponse(response);
        }
};

//std::atomic_int newNum(0);
//
//void* operator new(std::size_t size)
//{
//    newNum++;
//    std::cout << "New " << newNum << std::endl;
//    return std::malloc(size);
//}
//
//void operator delete(void* ptr)
//{
//    newNum--;
//    std::cout << "delete " << newNum << std::endl;
//    std::free(ptr);
//}


//int main(int /*argc*/,char** /*argv*/){
//    std::string cpp = "B(Safe<A> a,Safe<B> b,Safe<C> c)";
//    std::regex reg("B\\((.*?)\\)");
//    std::smatch match;
//    std::regex_search(cpp,match,reg);
//
//    std::smatch subMatch;
//    std::regex subReg("Safe<(.+?)+>");
//    std::string subCpp = match[1];
//    std::regex_search(subCpp,subMatch,subReg);
//    for(auto v : subMatch){
//        std::cout << subMatch.size() << " : " << v << std::endl;
//    }
//
////    std::regex_match(cpp,reg);
//    return 0;
//}

int main(int /*argc*/,char** /*argv*/){
//    Magic::g_TraceAppender = std::make_shared<>
//    std::printf("ptr[1] size of %llu \n",sizeof(Safe<A>));
//    ObjectWrapper<HttpRequest>
//    std::printf("ptr[2] size of %llu \n",sizeof(ObjectWrapper<A>));
//    std::printf("ptr[3] size of %llu \n",sizeof(ObjectWrapper<Magic::NetWork::Http::HttpRequest>));

    system("chcp 65001");

//    std::printf("size of %llu\n",sizeof(void*));

    Magic::NetWork::Http::Uri uri;
    uri.execute("mysql://admin@0.0.0.0:8181/xxx?password=12345678901a");
    std::printf("%s\n",uri.getUser().c_str());
    std::printf("%s\n",uri.getHost().c_str());
    std::printf("%s\n",&(uri.getPath().c_str()[1]));
    std::printf("%s\n",uri.getQuery().c_str());
    std::printf("%s\n",uri.getScheme().c_str());
    std::printf("%s\n",uri.getFragment().c_str());
    std::printf("%d\n",uri.getPort());

    Magic::Thread::SetName("Master");
    Magic::Configure([](const Safe<Magic::Container>& ioc){
        ioc->registerType<Magic::Logger,Safe<Magic::Config>>();
    });

    Safe<Magic::IConfigFormatter>  formatter = std::make_shared<Magic::InIConfigFormatter>();
    Safe<Magic::ConfigFile> configFile = std::make_shared<Magic::ConfigFile>(formatter);
    Safe<Magic::Config> config = std::make_shared<Magic::Config>(configFile);


    Magic::g_Logger = std::make_shared<Magic::Logger>(config);
    Safe<Magic::ILogAppender> logAppender = std::make_shared<Magic::StdOutLogAppender>();
    Magic::g_Logger->addILogAppender(logAppender);
    Safe<Magic::TimingWheel> timingWheel = std::make_shared<Magic::TimingWheel>(config);
    Safe<Magic::NetWork::IoPool> pool = std::make_shared<Magic::NetWork::IoPool>(config);
    timingWheel->run();
    Magic::NetWork::Http::HttpServer server(pool,timingWheel,config);


    Safe<Magic::NetWork::Http::IHttpServlet> resservlet = std::make_shared<ResourceServlet>();
    Safe<Magic::NetWork::Http::HttpServletDispatch> dispatch = std::make_shared<Magic::NetWork::Http::HttpServletDispatch>();

    MAGIC_DEBUG() << __cplusplus;
    MAGIC_WARN() << __cplusplus;
    MAGIC_ERROR() << __cplusplus;
    MAGIC_FATAL() << __cplusplus;

    dispatch->setHttpServlet(resservlet);
//    dispatch->addHttpServlet(resservlet);

    resservlet->addRoute("/",&ResourceServlet::handle1);
    resservlet->addRoute("/chat",&ResourceServlet::websocket);

    server.setServletDispatch(dispatch);
    server.run();
    return 0;
}

//int main(){
//    system("color");
//
//    printf("以下是测试文字：\n");
//    printf("\033[0m默认文字\033[0m\n");
//    printf("\033[1m高亮文字\033[0m\n");
//    printf("\033[2m低亮文字\033[0m\n");
//    //printf("\033[3m无效文字\033[0m\n");
//    printf("\033[4m下划线文字\033[0m\n");
//    printf("\033[5m闪烁文字(无效)\033[0m\n");
//    //printf("\033[6m无效文字\033[0m\n");
//    printf("\033[7m反显文字\033[0m\n");
//    printf("\033[8m消隐文字(无效)\033[0m\n");
//
//    printf("\n\033[31;1m字体颜色\033[0m测试文字\n");
//    printf("\033[30m低亮黑色文字2333\033[0m\t\033[30;1m高亮黑色文字2333\033\n[0m");
//    printf("\033[31m低亮红色文字\033[0m\t\033[31;1m高亮红色文字\033[0m\n");
//    printf("\033[32m低亮绿色文字\033[0m\t\033[32;1m高亮绿色文字\033[0m\n");
//    printf("\033[33m低亮黄色文字\033[0m\t\033[33;1m高亮黄色文字\033[0m\n");
//    printf("\033[34m低亮蓝色文字\033[0m\t\033[34;1m高亮蓝色文字\033[0m\n");
//    printf("\033[35m低亮紫色文字\033[0m\t\033[35;1m高亮紫色文字\033[0m\n");
//    printf("\033[36m低亮浅蓝文字\033[0m\t\033[36;1m高亮浅蓝文字\033[0m\n");
//    printf("\033[37m低亮白色文字\033[0m\t\033[37;1m高亮白色文字\033[0m\n");
//    printf("\033[38m测试文字\033[0m\n");
//    printf("\033[39m测试文字\033[0m\n");
//
//    printf("\n\033[31;1m背景颜色\033[0m测试文字\n");
//    printf("\033[40m低亮文字黑色背景\033[0m\t\033[40;1m高亮文字黑色背景\033[0m\n");
//    printf("\033[41m低亮文字红色背景\033[0m\t\033[41;1m高亮文字红色背景\033[0m\n");
//    printf("\033[42m低亮文字绿色背景\033[0m\t\033[42;1m高亮文字绿色背景\033[0m\n");
//    printf("\033[43m低亮文字黄色背景\033[0m\t\033[43;1m高亮文字黄色背景\033[0m\n");
//    printf("\033[44m低亮文字蓝色背景\033[0m\t\033[44;1m高亮文字蓝色背景\033[0m\n");
//    printf("\033[45m低亮文字紫色背景\033[0m\t\033[45;1m高亮文字紫色背景\033[0m\n");
//    printf("\033[46m低亮文字浅蓝背景\033[0m\t\033[46;1m高亮文字浅蓝背景\033[0m\n");
//    printf("\033[47m低亮文字白色背景\033[0m\t\033[47;1m高亮文字白色背景\033[0m\n");
//    printf("\033[48m测试文字\033[0m\n");
//    printf("\033[49m测试文字\033[0m\n");
//}
