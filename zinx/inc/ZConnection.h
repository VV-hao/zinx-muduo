#if !defined(ZINX_CONNECTION_H)
#define ZINX_CONNECTION_H

#include <muduo/TcpConnection.h>

namespace zinx {
    /// Use muduo::TcpConnectionPtr as ZinxConnection
    /// TODO: 
    ///     Wrap the ZinxConnection by muduo::TcpConnectionPtr to implement advanced function
    ///     such as TLS...
    using ZinxConnectionPtr = muduo::TcpConnectionPtr; 
} // namespace zinx 

#endif // ZINX_CONNECTION_H
