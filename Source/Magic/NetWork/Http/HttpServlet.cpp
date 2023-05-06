/*
 * @Author: INotFound
 * @Date: 2020-03-15 17:45:01
 * @LastEditTime: 2021-02-01 22:24:51
 */
#include <regex>
#include "Magic/Utilty/Trace.h"
#include "Magic/Utilty/Logger.h"
#include "Magic/NetWork/Http/HttpServlet.h"

namespace Magic{
namespace NetWork{
namespace Http{

    std::string GenerateHtml(const std::string_view& status,const std::string_view& title,const std::string_view& message){
        std::stringstream html;
        html << "<!DOCTYPE html><html lang=\"en\"><head><title>"
             << status
             << "</title></head><body><center><h1>"
             << title
             << "</h1><h3>"
             << message
             << "</h3></center><hr><center>Magic/2.0.0</center></body></html>";
        return html.str();
    }

    IHttpServlet::~IHttpServlet() = default;

    void HttpServletDispatch::handle(const Safe<HttpSocket>& httpSocket){
        std::lock_guard<std::mutex> locker(m_Mutex);
        std::string_view httpPath = httpSocket->getRequest()->getPath();
        try{
            auto exactlyIter = m_NormalRoutes.find(std::string(httpPath.data(),httpPath.size()));
            auto exactlyEnd = m_NormalRoutes.end();
            if(exactlyIter != exactlyEnd){
            #ifdef TRACE
                TraceTimer traceTimer(httpPath);
            #endif
                /// Global Before
                for(const auto& func : m_AspectBefores){
                    if(!func(httpSocket))
                        return;
                }
                const auto& aspectAfters = std::get<2>(exactlyIter->second);
                const auto& aspectBefores = std::get<1>(exactlyIter->second);
                /// Before
                for(const auto& func : aspectBefores){
                    if(!func(httpSocket))
                        return;
                }
                /// Handle
                std::get<0>(exactlyIter->second)(httpSocket);
                /// After
                for(const auto& func : aspectAfters){
                    if(!func(httpSocket))
                        return;
                }
                /// Global After
                for(const auto& func : m_AspectAfters){
                    if(!func(httpSocket))
                        return;
                }
                return;
            }

            auto matchIter = m_MatchRoutes.begin();
            auto matchEnd = m_MatchRoutes.end();
            std::regex reg;
            for(;matchIter != matchEnd;matchIter++){
                reg.assign(matchIter->first);
                if(std::regex_match(httpPath.begin(),httpPath.end(),reg)){
                #ifdef TRACE
                    TraceTimer traceTimer(httpPath);
                #endif
                    /// Global Before
                    for(const auto& func : m_AspectBefores){
                        if(!func(httpSocket))
                            return;
                    }
                    const auto& aspectAfters = std::get<2>(matchIter->second);
                    const auto& aspectBefores = std::get<1>(matchIter->second);
                    /// Before
                    for(const auto& func : aspectBefores){
                        if(!func(httpSocket))
                            return;
                    }
                    /// Handle
                    std::get<0>(matchIter->second)(httpSocket);
                    /// After
                    for(const auto& func : aspectAfters){
                        if(!func(httpSocket))
                            return;
                    }
                    /// Global After
                    for(const auto& func : m_AspectAfters){
                        if(!func(httpSocket))
                            return;
                    }
                    return;
                }
            }
        }catch(const std::exception& ec){
            MAGIC_WARN() << ec.what();
            const auto& response = httpSocket->getResponse();
            response->setStatus(HttpStatus::INTERNAL_SERVER_ERROR);
            response->setBody(GenerateHtml(HttpStatusToString(HttpStatus::INTERNAL_SERVER_ERROR),"Exception",ec.what()));
            httpSocket->sendResponse(response);
            return;
        }
        const auto& response = httpSocket->getResponse();
        response->setStatus(HttpStatus::NOT_FOUND);
        httpSocket->sendResponse(response);

    }

    void HttpServletDispatch::setHttpServlet(const Safe<IHttpServlet>& servlet){
        std::lock_guard<std::mutex> locker(m_Mutex);
        servlet->m_ServletDispatch = this->shared_from_this();
    }
}
}
}
