// #include <gtest/gtest.h>
// #include <unordered_map>
// #include <memory>
// #include <sys/epoll.h>
// #include <cstdint>
// #include <utils/macros.h>
// #include <ifaddrs.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <iterator>
// #include <cstddef>
// #include <cstdint>
// #include <arpa/inet.h>
// #include <fcntl.h>
// #include <netinet/tcp.h>
// #include <cstring>

// namespace
// {
//     class Interface
//     {
//     public:
//         Interface() noexcept
//             : m_Name(),
//               m_Host()
//         {
//         }

//         Interface(const std::string& iName, const std::string& iHost) noexcept
//             : m_Name(iName),
//               m_Host(iHost)
//         {
//         }

//         std::string m_Name;
//         std::string m_Host;
//     };

//     class InterfacesInfo
//     {
//     public:
//         InterfacesInfo()
//         {
//             this->BuildInterfaceList();
//         }

//         bool TryGetInterface(const std::string& iInterface, Interface& oInterface) const
//         {
//             for (const Interface& interface : this->m_Interfaces)
//             {
//                 if (interface.m_Name == iInterface ||
//                     interface.m_Host == iInterface)
//                 {
//                     oInterface = interface;

//                     return true;
//                 }
//             }

//             return false;
//         }

//     private:
//         void BuildInterfaceList()
//         {
//             ifaddrs* pInterfaceAddresses = nullptr;

//             if (-1 == getifaddrs(&pInterfaceAddresses))
//             {
//                 return;
//             }

//             char host[NI_MAXHOST];

//             for (ifaddrs* pInterfaceAddress = pInterfaceAddresses; nullptr != pInterfaceAddress; pInterfaceAddress = pInterfaceAddress->ifa_next)
//             {
//                 if (nullptr == pInterfaceAddress->ifa_addr)
//                 {
//                     continue;
//                 }

//                 if (AF_INET != pInterfaceAddress->ifa_addr->sa_family)
//                 {
//                     continue;
//                 }

//                 int result = getnameinfo(pInterfaceAddress->ifa_addr, sizeof(sockaddr_in), host, std::size(host), nullptr, 0, NI_NUMERICHOST);

//                 if (0 == result)
//                 {
//                     this->m_Interfaces.emplace_back(pInterfaceAddress->ifa_name, host);
//                 }
//             }

//             freeifaddrs(pInterfaceAddresses);
//         }

//         std::vector<Interface> m_Interfaces;
//     };

//     inline const InterfacesInfo GlobalInterfacesInfo;

//     struct SocketEvent
//     {
//         using CallbackT = bool (*)(SocketEvent*);

//         SocketEvent(CallbackT iCallback, void* iSocket)
//             : m_Callback(iCallback),
//               m_Socket(iSocket)
//         {
//         }

//         CallbackT m_Callback;
//         void* m_Socket;
//     };

//     class ListenerEpoll
//     {
//     public:
//         static constexpr const int MaxPolledEvent = 1024;

//         ListenerEpoll()
//             : m_FileDescriptor(epoll_create1(0))
//         {
//             if (-1 == this->m_FileDescriptor)
//             {
//                 throw std::runtime_error("Epoll failed!");
//             }
//         }

//         ~ListenerEpoll()
//         {
//             close(this->m_FileDescriptor);

//             for (auto&& [fileDescriptor, pEpollEvent] : this->m_EpollEvents)
//             {
//                 (void)fileDescriptor;

//                 delete static_cast<SocketEvent*>(pEpollEvent->data.ptr);
//             }
//         }

//         bool Register(int iFileDescriptor, SocketEvent::CallbackT iCallback, void* iSocket)
//         {
//             SocketEvent* pSocketEvent = new SocketEvent(iCallback, iSocket);

//             auto pEvent = std::make_unique<epoll_event>();

//             pEvent->events = EPOLLIN;
//             pEvent->data.ptr = pSocketEvent;

//             if (true == this->m_EpollEvents.emplace(iFileDescriptor, std::move(pEvent)).second)
//             {
//                 epoll_ctl(this->m_FileDescriptor, EPOLL_CTL_ADD, iFileDescriptor, pEvent.get());

//                 return true;
//             }
//             else
//             {
//                 delete pSocketEvent;

//                 return false;
//             }
//         }

//         bool Unregister(int iFileDescriptor)
//         {
//             if (auto it = this->m_EpollEvents.find(iFileDescriptor); std::end(m_EpollEvents) != it)
//             {
//                 epoll_ctl(this->m_FileDescriptor, EPOLL_CTL_DEL, iFileDescriptor, nullptr);

//                 delete static_cast<SocketEvent*>(it->second->data.ptr);

//                 this->m_EpollEvents.erase(it);

//                 return true;
//             }
//             else
//             {
//                 return false;
//             }
//         }

//         bool Wait()
//         {
//             epoll_event polledEvents[MaxPolledEvent];

//             int count = epoll_wait(this->m_FileDescriptor, polledEvents, static_cast<int>(MaxPolledEvent), 0);

//             bool result = false;

//             for (int i = 0; i < count; ++i)
//             {
//                 SocketEvent* pSocketEvent = reinterpret_cast<SocketEvent*>(polledEvents[i].data.ptr);

//                 result |= pSocketEvent->m_Callback(pSocketEvent);
//             }

//             return result;
//         }

//     private:
//         std::unordered_map<int, std::unique_ptr<epoll_event>> m_EpollEvents;
//         int m_FileDescriptor;
//     };

//     class Socket
//     {
//     public:
//         Socket(const std::string& iRemoteHost, std::size_t iRemoteService, const std::string& iLocalHost, std::size_t iLocalService)
//             : m_RemoteHost(iRemoteHost),
//               m_RemoteService(iRemoteService),
//               m_LocalHost(iLocalHost),
//               m_LocalService(iLocalService),
//               m_IsConnected(false)
//         {
//             this->m_RemoteInfo.sin_family = AF_INET;
//             this->m_RemoteInfo.sin_port = htons(this->m_RemoteService);

//             this->m_LocalInfo.sin_family = AF_INET;
//             this->m_LocalInfo.sin_port = htons(this->m_LocalService);
//         }

//         Socket(const Socket&) = delete;
//         Socket(Socket&&) = delete;
//         Socket& operator=(const Socket&) = delete;
//         Socket& operator=(Socket&&) = delete;
//         virtual ~Socket() = default;

//         virtual bool Connect(InterfacesInfo& iInterfacesInfo)
//         {
//             if (true == this->m_IsConnected)
//             {
//                 return true;
//             }

//             if (false == this->m_LocalHost.empty())
//             {
//                 int rv = inet_pton(this->m_LocalInfo.sin_family, this->m_LocalHost.c_str(), &this->m_LocalInfo.sin_addr);

//                 if (rv < 0)
//                 {
//                     return false;
//                 }
//             }
//             else
//             {
//                 this->m_LocalInfo.sin_addr.s_addr = INADDR_ANY;
//             }

//             if (false == this->m_RemoteHost.empty())
//             {
//                 int rv = inet_pton(this->m_RemoteInfo.sin_family, this->m_RemoteHost.c_str(), &this->m_RemoteInfo.sin_addr);

//                 if (rv < 0)
//                 {
//                     return false;
//                 }
//             }
//             else
//             {
//                 this->m_RemoteInfo.sin_addr.s_addr = INADDR_ANY;
//             }

//             return true == this->Create(iInterfacesInfo);
//         }

//         virtual bool Disconnect() = 0;
//         virtual std::uint64_t Send(const std::byte* iBuffer, std::size_t iLength) = 0;

//         bool IsConnected() const
//         {
//             return this->m_IsConnected;
//         }

//         bool IsMulticast() const
//         {
//             return true == IN_MULTICAST(ntohl(inet_addr(this->m_RemoteHost.c_str())));
//         }

//     protected:
//         virtual int GetSocketType() const = 0;
//         virtual bool Create(InterfacesInfo& iInterfacesInfo) = 0;
//         virtual bool Bind(sockaddr_in& iAddressInfo) = 0;

//         sockaddr_in m_RemoteInfo;
//         std::string m_RemoteHost;
//         std::size_t m_RemoteService;
//         sockaddr_in m_LocalInfo;
//         std::string m_LocalHost;
//         std::size_t m_LocalService;
//         bool m_IsConnected;
//     };

//     class SocketInet : public Socket
//     {
//     public:
//         SocketInet(const std::string& iRemoteHost, std::size_t iRemoteService, const std::string& iLocalHost, std::size_t iLocalService)
//             : Socket(iRemoteHost, iRemoteService, iLocalHost, iLocalService),
//               m_FileDescriptor(-1)
//         {
//         }

//         SocketInet(const SocketInet&) = delete;
//         SocketInet(SocketInet&&) = delete;
//         SocketInet& operator=(const SocketInet&) = delete;
//         SocketInet& operator=(SocketInet&&) = delete;
//         virtual ~SocketInet() = default;

//         virtual bool Disconnect() override
//         {
//             if (-1 == this->m_FileDescriptor)
//             {
//                 return true;
//             }

//             this->SetSocketOptionLinger();

//             if (0 == close(this->m_FileDescriptor))
//             {
//                 this->m_FileDescriptor = -1;
//                 this->m_IsConnected = false;
//             }
//             else
//             {
//                 return false;
//             }

//             return true;
//         }

//     protected:
//         virtual bool Create(InterfacesInfo& iInterfacesInfo) override
//         {
//             if (-1 != this->m_FileDescriptor)
//             {
//                 return true;
//             }

//             this->m_FileDescriptor = socket(this->m_RemoteInfo.sin_family, this->GetSocketType(), 0);

//             return -1 != this->m_FileDescriptor;
//         }

//         virtual bool Bind(sockaddr_in& iAddressInfo) override
//         {
//             if (-1 == this->m_FileDescriptor)
//             {
//                 return false;
//             }

//             return 0 == bind(this->m_FileDescriptor, reinterpret_cast<sockaddr*>(&iAddressInfo), sizeof(iAddressInfo));
//         }

//         bool SetSocketOptionReuseAddr()
//         {
//             if (-1 == this->m_FileDescriptor)
//             {
//                 return false;
//             }

//             int value = 1;

//             return 0 == setsockopt(this->m_FileDescriptor, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const std::byte*>(&value), sizeof(value));
//         }

//         bool SetSocketOptionNonBlocking()
//         {
//             if (-1 == this->m_FileDescriptor)
//             {
//                 return false;
//             }

//             int statusFlag = fcntl(this->m_FileDescriptor, F_GETFL, 0) | O_NONBLOCK;

//             return 0 == fcntl(this->m_FileDescriptor, F_SETFL, SO_REUSEADDR, statusFlag);
//         }

//         bool SetSocketOptionLinger()
//         {
//             if (-1 == this->m_FileDescriptor)
//             {
//                 return false;
//             }

//             linger value;

//             value.l_onoff = 1;
//             value.l_linger = 0;

//             return 0 == setsockopt(this->m_FileDescriptor, SOL_SOCKET, SO_LINGER, reinterpret_cast<const std::byte*>(&value), sizeof(value));
//         }

//         int m_FileDescriptor;
//     };

//     template <typename Callback>
//     class SocketInetTcp final : public SocketInet
//     {
//         SocketInetTcp(const std::string& iRemoteHost, std::size_t iRemoteService, const std::string& iLocalHost, std::size_t iLocalService)
//             : SocketInet(iRemoteHost, iRemoteService, iLocalHost, iLocalService)
//         {
//         }

//         SocketInetTcp(const SocketInetTcp&) = delete;
//         SocketInetTcp(SocketInetTcp&&) = delete;
//         SocketInetTcp& operator=(const SocketInetTcp&) = delete;
//         SocketInetTcp& operator=(SocketInetTcp&&) = delete;
//         virtual ~SocketInetTcp() = default;

//         virtual bool Connect(InterfacesInfo& iInterfacesInfo) final override
//         {
//             if (true == this->m_IsConnected)
//             {

//             }

//             return false;
//         }

//         virtual bool Disconnect() final override
//         {
//             return SocketInet::Disconnect();
//         }

//         virtual std::uint64_t Send(const std::byte* iBuffer, std::size_t iLength) final override
//         {
//             return 0;
//         }

//         static bool OnRead(SocketEvent* iSocketEvent);
//         static bool OnExcept(void* iSocketEvent);

//     protected:
//         virtual int GetSocketType() const final override
//         {
//             return SOCK_STREAM;
//         }

//         virtual bool Create(InterfacesInfo& iInterfacesInfo) final override
//         {
//             return false;
//         }

//         bool SetTcpOptionNoDelay()
//         {
//             if (-1 == this->m_FileDescriptor)
//             {
//                 return false;
//             }

//             int value = 1;

//             return 0 == setsockopt(this->m_FileDescriptor, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const std::byte*>(&value), sizeof(value));
//         }

//         bool SetTcpOptionQuickAck()
//         {
//             if (-1 == this->m_FileDescriptor)
//             {
//                 return false;
//             }

//             int value = 1;

//             return 0 == setsockopt(this->m_FileDescriptor, IPPROTO_TCP, TCP_QUICKACK, reinterpret_cast<const std::byte*>(&value), sizeof(value));
//         }

//     private:
//         bool Read()
//         {
//             this->SetTcpOptionQuickAck();

//             std::int64_t recvLength = recv(this->m_FileDescriptor, this->m_Buffer.data() + this->m_UnconsumedLength, std::size(this->m_Buffer) - this->m_UnconsumedLength, 0);

//             if (likely(recvLength > 0))
//             {
//                 std::size_t length = this->m_UnconsumedLength + recvLength;

//                 std::size_t consumed = this->ReadData(this->m_Buffer.data(), length);

//                 this->m_UnconsumedLength = length - consumed;

//                 if (consumed < length)
//                 {
//                     std::memmove(this->m_Buffer.data(), this->m_Buffer.data() + consumed, this->m_UnconsumedLength);
//                 }

//                 return consumed > 0;
//             }
//             else if (unlikely(0 == recvLength))
//             {
//                 this->Disconnect();
//             }
//             else if (likely(EAGAIN == errno || EWOULDBLOCK == errno || EINPROGRESS == errno))
//             {
//                 errno = 0;
//             }
//             else
//             {
//                 this->Disconnect();
//             }

//             return false;
//         }

//         std::size_t ReadData(std::byte* iBuffer, std::size_t iLength)
//         {
//             std::size_t consumed = 0;

//             do
//             {
//                 std::size_t consumedFromProcessing = this->m_Callback(iBuffer + consumed, iLength - consumed);

//                 if (unlikely(0 == consumedFromProcessing))
//                 {
//                     break;
//                 }

//                 consumed += consumedFromProcessing;
//             }
//             while (consumed < iLength);

//             return consumed;
//         }

//         Callback& m_Callback;
//         std::size_t m_UnconsumedLength;
//         std::array<std::byte, 65535> m_Buffer;
//     };
// }

// TEST(NetworkingTest, InterfacesInfo)
// {
//     Interface interface;

//     EXPECT_TRUE(GlobalInterfacesInfo.TryGetInterface("127.0.0.1", interface));
// }
