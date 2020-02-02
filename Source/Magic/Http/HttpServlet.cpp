#include <regex>
#include "Http/HttpServlet.h"

namespace Magic{
namespace Http{

    HttpServlet::HttpServlet(const std::string& name)
        :m_Name{name}{
    }
    const std::string HttpServlet::getName() const{
        return m_Name;
    }

    HttpServletDispatch::HttpServletDispatch()
        :HttpServlet{"HttpServletDispatch"}{
    }
    void HttpServletDispatch::setDeafultServlet(MagicPtr<HttpServlet>& servlet){
        RWMutexWriteLock lock{m_Mutex};
        m_DeafultServlet = std::move(servlet);
    }

    void HttpServletDispatch::addServlet(const std::string& name,MagicPtr<HttpServlet>& servlet){
        RWMutexWriteLock lock{m_Mutex};
        m_Servlets.emplace(name,std::move(servlet));
    }
    void HttpServletDispatch::addGlobServlet(const std::string& name,MagicPtr<HttpServlet>& servlet){
        RWMutexWriteLock lock{m_Mutex};
        m_GlobServlets.emplace(name,std::move(servlet));
    }
    void HttpServletDispatch::handle(const std::shared_ptr<HttpSession>& session,MagicPtr<HttpRequest>& request,MagicPtr<HttpResponse>& response){
        auto& servlet = getMatchedServlet(request->getUrlPath());
        if(servlet){
            servlet->handle(session,request,response);
        }
    }
    MagicPtr<HttpServlet>& HttpServletDispatch::getMatchedServlet(const std::string& name){
        RWMutexWriteLock lock{m_Mutex};
        auto exactlyIter = m_Servlets.find(name);
        if(exactlyIter != m_Servlets.end()){
            return exactlyIter->second;
        }
        std::regex reg;
        auto globIter = m_GlobServlets.begin();
        auto globEnd = m_GlobServlets.begin();
        for(; globIter != globEnd; globIter++){
            reg.assign(globIter->first);
            if(std::regex_match(name,reg)){
                return globIter->second;
            }
        }
        return m_DeafultServlet;
    }
}
}