#include "logAppender.h"
#include "logFormatter.h"

using namespace Magic;

void StdOutLogAppender::log(LogLevel::Level level, std::shared_ptr<LogEvent> event){
    if(!this->m_Formatter){
        std::cout << "<(ERROR)> : " <<  std::endl;
        return;
    }
    this->m_Formatter->format(std::cout,level,event);
}

FileLogAppender::FileLogAppender(const std::string &path):m_Path(path){
    this->reOpen();
}

void FileLogAppender::log(LogLevel::Level level, std::shared_ptr<LogEvent> event) {
    this->m_Formatter->format(this->m_FileStream,level,event);
}

bool FileLogAppender::reOpen(){
    if(this->m_FileStream){
        this->m_FileStream.close();
	}
    this->m_FileStream.open(this->m_Path,std::ios_base::out | std::ios_base::app);
    return this->m_FileStream.is_open();
}
