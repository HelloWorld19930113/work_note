# IOCP模型与网络编程
2012-05-09 12:51 87336人阅读 

一。前言：
 在老师分配任务（“尝试利用IOCP模型写出服务端和客户端的代码”）给我时，脑子一片空白，并不知道什么是IOCP模型，会不会是像软件设计模式里面的工厂模式，装饰模式之类的那些呢？嘿嘿，不过好像是一个挺好玩的东西，挺好奇是什么东西来的，又是一个新知识啦~于是，开始去寻找一大堆的资料，为这个了解做准备，只是呢，有时还是想去找一本书去系统地学习一下，毕竟网络的资料还是有点零散。话说，本人学习这个模型的基础是，写过一个简单的Socket服务器及客户端程序，外加一个简单的Socket单服务器对多客户端程序，懂一点点的操作系统原理的知识。于是，本着一个学习与应用的态度开始探究这个IOCP是个什么东西。
 
二。提出相关问题：
1.  IOCP模型是什么？
2.  IOCP模型是用来解决什么问题的？它为什么存在？
3.  使用IOCP模型需要用到哪些知识？
4.  如何使用IOCP模型与Socket网络编程结合起来？
5.  学会了这个模型以后与我之前写过的简单的socket程序主要有哪些不同点？
 
三。部分问题探究及解决：（绝大多数是个人理解，再加上个人是菜鸟，如果有什么不对的地方，欢迎指正）
1.  什么是IOCP？什么是IOCP模型？IOCP模型有什么作用？
  1) IOCP（I/O Completion Port）,常称I/O完成端口。
  2) IOCP模型属于一种通讯模型，适用于(能控制并发执行的)高负载服务器的一个技术。
  3) 通俗一点说，就是用于高效处理很多很多的客户端进行数据交换的一个模型。
  4) 或者可以说，就是能异步I/O操作的模型。
  5) 只是了解到这些会让人很糊涂，因为还是不知道它究意具体是个什么东东呢？

下面我想给大家看三个图：
第一个是IOCP的内部工作队列图。(整合于《IOCP本质论》文章，在英文的基础上加上中文对照)
 
第二个是程序实现IOCP模型的基本步骤。(整合于《深入解释IOCP》，加个人观点、理解、翻译)
 
 
第三个是使用了IOCP模型及没使用IOCP模型的程序流程图。(个人理解绘制)
 
 
2.  IOCP的存在理由（IOCP的优点）及技术相关有哪些？
 之前说过，很通俗地理解可以理解成是用于高效处理很多很多的客户端进行数据交换的一个模型，那么，它具体的优点有些什么呢？它到底用到了哪些技术了呢？在Windows环境下又如何去使用这些技术来编程呢？它主要使用上哪些API函数呢？呃~看来我真是一个问题多多的人，跟前面提出的相关问题变种延伸了不少的问题，好吧，下面一个个来解决。
 
1) 使用IOCP模型编程的优点
       ① 帮助维持重复使用的内存池。(与重叠I/O技术有关)
       ② 去除删除线程创建/终结负担。
       ③ 利于管理，分配线程，控制并发，最小化的线程上下文切换。
       ④ 优化线程调度，提高CPU和内存缓冲的命中率。
2) 使用IOCP模型编程汲及到的知识点（无先后顺序）
       ① 同步与异步
       ② 阻塞与非阻塞
       ③ 重叠I/O技术
       ④ 多线程
       ⑤ 栈、队列这两种基本的数据结构
3) 需要使用上的API函数
  ① 与SOCKET相关
       1、链接套接字动态链接库：int WSAStartup(...);
       2、创建套接字库：        SOCKET socket(...);
       3、绑字套接字：          int bind(...);
       4、套接字设为监听状态：　int listen(...);
       5、接收套接字：          SOCKET accept(...);
       6、向指定套接字发送信息：int send(...);
       7、从指定套接字接收信息：int recv(...);
  ② 与线程相关
       1、创建线程：HANDLE CreateThread(...);
  ③ 重叠I/O技术相关
       1、向套接字发送数据：    int WSASend(...);
       2、向套接字发送数据包：  int WSASendFrom(...);
       3、从套接字接收数据：    int WSARecv(...);
       4、从套接字接收数据包：  int WSARecvFrom(...);
  ④ IOCP相关
       1、创建Completion端口： HANDLE WINAPI CreateIoCompletionPort(...);
       2、关联Completion端口： HANDLE WINAPI CreateIoCompletionPort(...);
       3、获取队列Completion状态: BOOL WINAPI GetQueuedCompletionStatus(...);
       4、投递一个队列Completion状态：BOOL WINAPI PostQueuedCompletionStatus(...);
 
四。完整的简单的IOCP服务器与客户端代码实例：
```cpp
// IOCP_TCPIP_Socket_Server.cpp  
  
#include <WinSock2.h>  
#include <Windows.h>  
#include <vector>  
#include <iostream>  
  
using namespace std;  
  
#pragma comment(lib, "Ws2_32.lib")      // Socket编程需用的动态链接库  
#pragma comment(lib, "Kernel32.lib")    // IOCP需要用到的动态链接库  
  
/** 
 * 结构体名称：PER_IO_DATA 
 * 结构体功能：重叠I/O需要用到的结构体，临时记录IO数据 
 **/  
const int DataBuffSize  = 2 * 1024;  
typedef struct  
{  
    OVERLAPPED overlapped;  
    WSABUF databuff;  
    char buffer[ DataBuffSize ];  
    int BufferLen;  
    int operationType;  
}PER_IO_OPERATEION_DATA, *LPPER_IO_OPERATION_DATA, *LPPER_IO_DATA, PER_IO_DATA;  
  
/** 
 * 结构体名称：PER_HANDLE_DATA 
 * 结构体存储：记录单个套接字的数据，包括了套接字的变量及套接字的对应的客户端的地址。 
 * 结构体作用：当服务器连接上客户端时，信息存储到该结构体中，知道客户端的地址以便于回访。 
 **/  
typedef struct  
{  
    SOCKET socket;  
    SOCKADDR_STORAGE ClientAddr;  
}PER_HANDLE_DATA, *LPPER_HANDLE_DATA;  
  
// 定义全局变量  
const int DefaultPort = 6000;         
vector < PER_HANDLE_DATA* > clientGroup;      // 记录客户端的向量组  
  
HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);  
DWORD WINAPI ServerWorkThread(LPVOID CompletionPortID);  
DWORD WINAPI ServerSendThread(LPVOID IpParam);  
  
// 开始主函数  
int main()  
{  
// 加载socket动态链接库  
    WORD wVersionRequested = MAKEWORD(2, 2); // 请求2.2版本的WinSock库  
    WSADATA wsaData;    // 接收Windows Socket的结构信息  
    DWORD err = WSAStartup(wVersionRequested, &wsaData);  
  
    if (0 != err){  // 检查套接字库是否申请成功  
        cerr << "Request Windows Socket Library Error!\n";  
        system("pause");  
        return -1;  
    }  
    if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2){// 检查是否申请了所需版本的套接字库  
        WSACleanup();  
        cerr << "Request Windows Socket Version 2.2 Error!\n";  
        system("pause");  
        return -1;  
    }  
  
// 创建IOCP的内核对象  
    /** 
     * 需要用到的函数的原型： 
     * HANDLE WINAPI CreateIoCompletionPort( 
     *    __in   HANDLE FileHandle,     // 已经打开的文件句柄或者空句柄，一般是客户端的句柄 
     *    __in   HANDLE ExistingCompletionPort, // 已经存在的IOCP句柄 
     *    __in   ULONG_PTR CompletionKey,   // 完成键，包含了指定I/O完成包的指定文件 
     *    __in   DWORD NumberOfConcurrentThreads // 真正并发同时执行最大线程数，一般推介是CPU核心数*2 
     * ); 
     **/  
    HANDLE completionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0);  
    if (NULL == completionPort){    // 创建IO内核对象失败  
        cerr << "CreateIoCompletionPort failed. Error:" << GetLastError() << endl;  
        system("pause");  
        return -1;  
    }  
  
// 创建IOCP线程--线程里面创建线程池  
  
    // 确定处理器的核心数量  
    SYSTEM_INFO mySysInfo;  
    GetSystemInfo(&mySysInfo);  
  
    // 基于处理器的核心数量创建线程  
    for(DWORD i = 0; i < (mySysInfo.dwNumberOfProcessors * 2); ++i){  
        // 创建服务器工作器线程，并将完成端口传递到该线程  
        HANDLE ThreadHandle = CreateThread(NULL, 0, ServerWorkThread, completionPort, 0, NULL);  
        if(NULL == ThreadHandle){  
            cerr << "Create Thread Handle failed. Error:" << GetLastError() << endl;  
        system("pause");  
            return -1;  
        }  
        CloseHandle(ThreadHandle);  
    }  
  
// 建立流式套接字  
    SOCKET srvSocket = socket(AF_INET, SOCK_STREAM, 0);  
  
// 绑定SOCKET到本机  
    SOCKADDR_IN srvAddr;  
    srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);  
    srvAddr.sin_family = AF_INET;  
    srvAddr.sin_port = htons(DefaultPort);  
    int bindResult = bind(srvSocket, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR));  
    if(SOCKET_ERROR == bindResult){  
        cerr << "Bind failed. Error:" << GetLastError() << endl;  
        system("pause");  
        return -1;  
    }  
  
// 将SOCKET设置为监听模式  
    int listenResult = listen(srvSocket, 10);  
    if(SOCKET_ERROR == listenResult){  
        cerr << "Listen failed. Error: " << GetLastError() << endl;  
        system("pause");  
        return -1;  
    }  
      
// 开始处理IO数据  
    cout << "本服务器已准备就绪，正在等待客户端的接入...\n";  
  
    // 创建用于发送数据的线程  
    HANDLE sendThread = CreateThread(NULL, 0, ServerSendThread, 0, 0, NULL);  
  
    while(true){  
        PER_HANDLE_DATA * PerHandleData = NULL;  
        SOCKADDR_IN saRemote;  
        int RemoteLen;  
        SOCKET acceptSocket;  
  
        // 接收连接，并分配完成端，这儿可以用AcceptEx()  
        RemoteLen = sizeof(saRemote);  
        acceptSocket = accept(srvSocket, (SOCKADDR*)&saRemote, &RemoteLen);  
        if(SOCKET_ERROR == acceptSocket){   // 接收客户端失败  
            cerr << "Accept Socket Error: " << GetLastError() << endl;  
            system("pause");  
            return -1;  
        }  
          
        // 创建用来和套接字关联的单句柄数据信息结构  
        PerHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA));  // 在堆中为这个PerHandleData申请指定大小的内存  
        PerHandleData -> socket = acceptSocket;  
        memcpy (&PerHandleData -> ClientAddr, &saRemote, RemoteLen);  
        clientGroup.push_back(PerHandleData);       // 将单个客户端数据指针放到客户端组中  
  
        // 将接受套接字和完成端口关联  
        CreateIoCompletionPort((HANDLE)(PerHandleData -> socket), completionPort, (DWORD)PerHandleData, 0);  
  
          
        // 开始在接受套接字上处理I/O使用重叠I/O机制  
        // 在新建的套接字上投递一个或多个异步  
        // WSARecv或WSASend请求，这些I/O请求完成后，工作者线程会为I/O请求提供服务      
        // 单I/O操作数据(I/O重叠)  
        LPPER_IO_OPERATION_DATA PerIoData = NULL;  
        PerIoData = (LPPER_IO_OPERATION_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_OPERATEION_DATA));  
        ZeroMemory(&(PerIoData -> overlapped), sizeof(OVERLAPPED));  
        PerIoData->databuff.len = 1024;  
        PerIoData->databuff.buf = PerIoData->buffer;  
        PerIoData->operationType = 0;    // read  
  
        DWORD RecvBytes;  
        DWORD Flags = 0;  
        WSARecv(PerHandleData->socket, &(PerIoData->databuff), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);  
    }  
  
    system("pause");  
    return 0;  
}  
  
// 开始服务工作线程函数  
DWORD WINAPI ServerWorkThread(LPVOID IpParam)  
{  
    HANDLE CompletionPort = (HANDLE)IpParam;  
    DWORD BytesTransferred;  
    LPOVERLAPPED IpOverlapped;  
    LPPER_HANDLE_DATA PerHandleData = NULL;  
    LPPER_IO_DATA PerIoData = NULL;  
    DWORD RecvBytes;  
    DWORD Flags = 0;  
    BOOL bRet = false;  
  
    while(true){  
        bRet = GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&PerHandleData, (LPOVERLAPPED*)&IpOverlapped, INFINITE);  
        if(bRet == 0){  
            cerr << "GetQueuedCompletionStatus Error: " << GetLastError() << endl;  
            return -1;  
        }  
        PerIoData = (LPPER_IO_DATA)CONTAINING_RECORD(IpOverlapped, PER_IO_DATA, overlapped);  
          
        // 检查在套接字上是否有错误发生  
        if(0 == BytesTransferred){  
            closesocket(PerHandleData->socket);  
            GlobalFree(PerHandleData);  
            GlobalFree(PerIoData);  
            continue;  
        }  
          
        // 开始数据处理，接收来自客户端的数据  
        WaitForSingleObject(hMutex,INFINITE);  
        cout << "A Client says: " << PerIoData->databuff.buf << endl;  
        ReleaseMutex(hMutex);  
  
        // 为下一个重叠调用建立单I/O操作数据  
        ZeroMemory(&(PerIoData->overlapped), sizeof(OVERLAPPED)); // 清空内存  
        PerIoData->databuff.len = 1024;  
        PerIoData->databuff.buf = PerIoData->buffer;  
        PerIoData->operationType = 0;    // read  
        WSARecv(PerHandleData->socket, &(PerIoData->databuff), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL);  
    }  
  
    return 0;  
}  
  
  
// 发送信息的线程执行函数  
DWORD WINAPI ServerSendThread(LPVOID IpParam)  
{  
    while(1){  
        char talk[200];  
        gets(talk);  
        int len;  
        for (len = 0; talk[len] != '\0'; ++len){  
            // 找出这个字符组的长度  
        }  
        talk[len] = '\n';  
        talk[++len] = '\0';  
        printf("I Say:");  
        cout << talk;  
        WaitForSingleObject(hMutex,INFINITE);  
        for(int i = 0; i < clientGroup.size(); ++i){  
            send(clientGroup[i]->socket, talk, 200, 0);  // 发送信息  
        }  
        ReleaseMutex(hMutex);   
    }  
    return 0;  
}  
```
 
```cpp
// IOCP_TCPIP_Socket_Client.cpp  
  
#include <iostream>  
#include <cstdio>  
#include <string>  
#include <cstring>  
#include <winsock2.h>  
#include <Windows.h>  
  
using namespace std;  
  
#pragma comment(lib, "Ws2_32.lib")      // Socket编程需用的动态链接库  
  
SOCKET sockClient;      // 连接成功后的套接字  
HANDLE bufferMutex;     // 令其能互斥成功正常通信的信号量句柄  
const int DefaultPort = 6000;  
  
int main()  
{  
// 加载socket动态链接库(dll)  
    WORD wVersionRequested;  
    WSADATA wsaData;    // 这结构是用于接收Wjndows Socket的结构信息的  
    wVersionRequested = MAKEWORD( 2, 2 );   // 请求2.2版本的WinSock库  
    int err = WSAStartup( wVersionRequested, &wsaData );  
    if ( err != 0 ) {   // 返回值为零的时候是表示成功申请WSAStartup  
        return -1;  
    }  
    if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) { // 检查版本号是否正确  
        WSACleanup( );  
        return -1;   
    }  
      
// 创建socket操作，建立流式套接字，返回套接字号sockClient  
     sockClient = socket(AF_INET, SOCK_STREAM, 0);  
     if(sockClient == INVALID_SOCKET) {   
        printf("Error at socket():%ld\n", WSAGetLastError());   
        WSACleanup();   
        return -1;   
      }   
  
// 将套接字sockClient与远程主机相连  
    // int connect( SOCKET s,  const struct sockaddr* name,  int namelen);  
    // 第一个参数：需要进行连接操作的套接字  
    // 第二个参数：设定所需要连接的地址信息  
    // 第三个参数：地址的长度  
    SOCKADDR_IN addrSrv;  
    addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");      // 本地回路地址是127.0.0.1;   
    addrSrv.sin_family = AF_INET;  
    addrSrv.sin_port = htons(DefaultPort);  
    while(SOCKET_ERROR == connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR))){  
        // 如果还没连接上服务器则要求重连  
        cout << "服务器连接失败，是否重新连接？（Y/N):";  
        char choice;  
        while(cin >> choice && (!((choice != 'Y' && choice == 'N') || (choice == 'Y' && choice != 'N')))){  
            cout << "输入错误，请重新输入:";  
            cin.sync();  
            cin.clear();  
        }  
        if (choice == 'Y'){  
            continue;  
        }  
        else{  
            cout << "退出系统中...";  
            system("pause");  
            return 0;  
        }  
    }  
    cin.sync();  
    cout << "本客户端已准备就绪，用户可直接输入文字向服务器反馈信息。\n";  
  
    send(sockClient, "\nAttention: A Client has enter...\n", 200, 0);  
  
    bufferMutex = CreateSemaphore(NULL, 1, 1, NULL);   
  
    DWORD WINAPI SendMessageThread(LPVOID IpParameter);  
    DWORD WINAPI ReceiveMessageThread(LPVOID IpParameter);  
  
    HANDLE sendThread = CreateThread(NULL, 0, SendMessageThread, NULL, 0, NULL);    
    HANDLE receiveThread = CreateThread(NULL, 0, ReceiveMessageThread, NULL, 0, NULL);    
  
         
    WaitForSingleObject(sendThread, INFINITE);  // 等待线程结束  
    closesocket(sockClient);  
    CloseHandle(sendThread);  
    CloseHandle(receiveThread);  
    CloseHandle(bufferMutex);  
    WSACleanup();   // 终止对套接字库的使用  
  
    printf("End linking...\n");  
    printf("\n");  
    system("pause");  
    return 0;  
}  
  
  
DWORD WINAPI SendMessageThread(LPVOID IpParameter)  
{  
    while(1){  
        string talk;  
        getline(cin, talk);  
        WaitForSingleObject(bufferMutex, INFINITE);     // P（资源未被占用）    
        if("quit" == talk){  
            talk.push_back('\0');  
            send(sockClient, talk.c_str(), 200, 0);  
            break;  
        }  
        else{  
            talk.append("\n");  
        }  
        printf("\nI Say:(\"quit\"to exit):");  
        cout << talk;  
        send(sockClient, talk.c_str(), 200, 0); // 发送信息  
        ReleaseSemaphore(bufferMutex, 1, NULL);     // V（资源占用完毕）   
    }  
    return 0;  
}  
  
  
DWORD WINAPI ReceiveMessageThread(LPVOID IpParameter)  
{  
    while(1){     
        char recvBuf[300];  
        recv(sockClient, recvBuf, 200, 0);  
        WaitForSingleObject(bufferMutex, INFINITE);     // P（资源未被占用）    
  
        printf("%s Says: %s", "Server", recvBuf);       // 接收信息  
          
        ReleaseSemaphore(bufferMutex, 1, NULL);     // V（资源占用完毕）   
    }  
    return 0;  
}  
```
 

五。本次学习资料
       几翻周折，终于写出一个比较简单的IOCP模型的服务器与客户端啦，并且也大概了解这个模型的思路啦~没有买书的娃，伤不起啊，只能从网上搜罗资料，幸好有这些文章在，最后为下列这些文章的作者说声谢谢~

 
 
顶
54
踩
1
 
 
上一篇Windows Socket 编程_单个服务器对多个客户端简单通讯
下一篇本地计算机网络信息的获取
  相关文章推荐
• C++ SOCKET通信模型（四）IOCP
• Hadoop生态系统零基础入门
• 博客文章《完成端口详解》配套代码
• 系统集成工程师必过冲刺！
• Client IOCP.zip
• 征服React Native我有妙招
• 博客文章《完成端口详解》配套代码(压力测试客户端)
• FFmpeg音视频高级开发实战
• Windows高性能大并发IOCP
• 5天搞定深度学习框架-Caffe
• IOCP 浅析与实例
• Python数据分析经典案例解析
• 高效稳定完成端口IOCP动态库VC6.0版
• 晨晨网络留言板
• Windows下两种iocp实现的差距
• Windows高性能并发IOCP之DELPHI实现

查看评论
22楼 瀚度 2016-02-16 09:54发表 [回复]

厉害啊，打算有时间研究下完成端口呢
21楼 仕隆 2015-04-10 00:14发表 [回复]

谢谢分享
20楼 shijihuangshou 2015-02-05 15:34发表 [回复]

谢谢博主分享，最后的学习资料能共享一下么
19楼 某种意境 2015-01-08 11:25发表 [回复]

确实很好的一篇文章啊，你当时写的时候还是学生，想想真的差别好大，确实很用心啊，文章内容丰富全面，难得一见的好文啊。项目中有用到IOCP模型，这翻出来真不赖
18楼 xf1314wc 2014-11-12 09:00发表 [回复]

谢谢共享
17楼 wangfenghui132 2014-10-27 21:50发表 [回复]

博主你好，我对通过GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (PULONG_PTR)&PerHandleData, (LPOVERLAPPED*)&IpOverlapped, INFINITE)获得WSARecv(PerHandleData->socket, &(PerIoData->databuff), 1, &RecvBytes, &Flags, &(PerIoData->overlapped), NULL)函数中的overlapped所在的地址不是很理解。我就是奇怪通过WSARecv函数获得的PerIoData->overlapped怎么就能通过GetQueuedCompletionStatus在完成端口里面找到呢？我不太清楚他们是怎么关联起来的。也是初学IOCP，有好多不懂得。可能我表达的不是很清楚，不知道楼主能不能解释一下，先谢谢了
16楼 苏客 2014-09-12 16:53发表 [回复]

楼主可否留个QQ，私下里交流一下，很倾佩您的个人编程能力，最近也在搞这方面的研究，有几个地方不太明白，所以想向您学习一下，先谢谢您的学习资料了，帮助特别大。
15楼 苏客 2014-09-12 16:52发表 [回复]

楼主可否留个QQ，私下里交流一下，很倾佩您的个人编程能力，最近也在搞这方面的研究，有几个地方不太明白，所以想向您学习一下，先谢谢您的学习资料了，帮助特别大。
14楼 Kiven 2014-08-14 13:59发表 [回复]

楼主，你好厉害啊
13楼 Kiven 2014-08-14 13:58发表 [回复]

崇拜啊！
12楼 groundhappy 2014-07-12 23:55发表 [回复]

overlpped中没有指定hEvent为何还能成功？我没有指定hEvent就WSARecv error返回 10045
11楼 X30513804 2014-03-21 09:41发表 [回复]

mark
10楼 dahaiI0 2014-02-11 15:05发表 [回复]

博文不错，不错客户端的发送代码有点问题，应该是 int nRes = send(sockClient, talk.c_str(), talk.length(), 0); 如果参数写200有可能发送失败
Re: oldmtn 2014-04-04 10:11发表 [回复]

回复dahaiI0：Very good!,,我加上了talk.length()就OK了。。
9楼 wumingzcj 2013-10-08 11:43发表 [回复]

为什么用GlobalAlloc 分配堆内存，用new 可以吧？
8楼 刘沄 2013-05-02 21:58发表 [回复]

想问一下博主，你的代码里面是否没有考虑释放GlobalAlloc分配的内存？
Re: neicole 2013-05-03 12:24发表 [回复]

回复刘沄：GlobalAlloc分配内存有两次，主要是在服务器刚刚接收到客户端时，1.保存client的数据所使用；（代码146行）2.刚接收到数据时，数据的保存临时处理；（代码160行）而释放这些内存是在服务器与客户端连接断开的时候，代码146行的内存释放对应代码198行，代码160行的内在释放对应代码199行。
Re: 刘沄 2013-05-15 22:01发表 [回复]

回复neicole：这个只是当客户端发起断开连接的情况下吧，如果你得服务器先关闭是不是就内存泄漏了？
Re: neicole 2013-05-19 11:50发表 [回复]

回复刘沄：是喔，怎么没有想到这方面~这样的话，我想要不要这样子设一下标记，客户端的数据结构（自定义）加上缓冲区的地址。每次连接上ClientSocket后，将ClientSocket存于ClientSocketList中，当服务器主动断开时，对ClientSocket检测，释放ClientSocket对应的Buffer，删除Buffer，closeSocket。
Re: moshang005 2017-05-28 23:20发表 [回复]

回复neicole：大神想问下服务端 
数据接收 
// 开始数据处理，接收来自客户端的数据 
WaitForSingleObject(hMutex,INFINITE); 
cout << "A Client says: " << PerIoData->databuff.buf << endl; 
ReleaseMutex(hMutex); 
还有数据发送
WaitForSingleObject(hMutex,INFINITE); 
for(int i = 0; i < clientGroup.size(); ++i){ 
send(clientGroup[i]->socket, talk, 200, 0); // 发送信息 
} 
ReleaseMutex(hMutex); 
这个锁是什么作用？是不能同时对一个socket读写吗？
Re: vvqboy 2015-05-07 17:22发表 [回复]

有客户端断开连接的时候,是不是应该把保存客户端的vector也清理下.
[cpp] view plain copy print?
// 检查在套接字上是否有错误发生  
        if (0 == BytesTransferred){  
            cout << "客户端已经关闭连接" << endl;  
            closesocket(PerHandleData->socket);  
            GlobalFree(PerHandleData);  
            GlobalFree(PerIoData);  
            std::vector<LPPER_HANDLE_DATA>::iterator pos;  
            pos = find(clientGroup.begin(), clientGroup.end(),  
                PerHandleData);  
            if (pos != clientGroup.end()) {  
                clientGroup.erase(pos);  
            }  
            continue;  
        }  
Re: akuo430 2015-08-06 11:33发表 [回复]

回复vvqboy：请问你全看懂了吗？能否帮忙解释下服务端的发送线程。初学者看不懂，
for(int i = 0; i < clientGroup.size(); ++i){ 
send(clientGroup[i]->socket, talk, 200, 0); // 发送信息 
中间clientGroup[i]->socket是哪里来的
7楼 qianluhust 2013-02-28 09:37发表 [回复]

楼主可否请教一下，IOCP模型一定要用在windows2000活winnet环境下么？谢谢
Re: neicole 2013-03-03 08:24发表 [回复]

回复qianluhust：其实运行环境的话，我是在WinXp和Win7做过都没有问题。
至于windows2000的话就没有试过，不过想知道答案的话，可以转换一下问题，哪些环境支持WindowsAPI的WinSock库及与IOCP相关函数的使用的。能支持这些库的，自然IOCP的运行也不成问题了。
6楼 jimoyangguang 2013-01-08 13:42发表 [回复]

谢谢lz分享
Re: neicole 2013-01-08 16:37发表 [回复]

回复jimoyangguang：:)
5楼 zxpyld3x 2012-11-12 12:54发表 [回复]

能不能更新成acceptex()函数
Re: lshmusic 2014-03-04 09:44发表 [回复]

回复zxpyld3x：http://blog.csdn.net/piggyxp/article/details/6922277
ICOP AcceptEx 经典教程
Re: neicole 2012-11-15 01:02发表 [回复]

回复zxpyld3x：不好意思，这函数没怎么研究过。近期也比较多事情做。如果这童鞋可以的话，如果做出来了，记得一起共享一下哦~
4楼 唐伯虎傻画画 2012-10-24 20:06发表 [回复]

我擦 LZ 我太爱你了 我最近对这个特痴狂 哈哈 终于找到 服务器和客户端的了 ，我写了一个IOCP的服务器模型，但是客户端可以连接但是不能发数据，我收藏了，不介意吧，哈哈，最后说句谢谢
Re: neicole 2012-10-28 21:37发表 [回复]

回复唐伯虎傻画画：哈哈，不介意~　看见你的话语，觉得你对学术的问题好有激情哦~
3楼 woxihuanqingjiao 2012-09-08 19:41发表 [回复]

楼主能给整个项目？我学习下 810776886@qq.com
Re: neicole 2012-09-08 22:10发表 [回复]

回复woxihuanqingjiao：不，项目不能发滴~　因为不是自己做滴~　
不好意思罗~　　还有就是。。。　项目还在进行当中哦~
Re: neicole 2012-09-08 22:11发表 [回复]

回复neicole：不是完全自己做才对。。。。　团队。
2楼 mwxlogin 2012-08-26 21:56发表 [回复]

博主你好，这篇博文对我帮助很大，非常感谢

顺便我想问下楼主画图用的是什么软件，图画得very nice
Re: neicole 2012-08-27 08:31发表 [回复]

回复mwxlogin：嗯，有收获就好~用的是EDrawMax呢，很好用的软件~
1楼 OKILOVE 2012-05-30 15:27发表 [回复]

Hi neicole

我试验了一下，从client往server发送信息居然收不到，GetQueuedCompletionStatus没有取消阻塞，你那有这样的情况吗
Re: neicole 2012-05-31 09:15发表 [回复]

回复OKILOVE：嘿嘿，mark一下，184行的Flags = 0不能漏.

查资料时，网友道：
不是MSG_PEEK，MSG_OOB ，MSG_PARTIAL
所以就取0，这似乎是没什么道理，但winsocket有好多东西在文档里是没有描述的，这是《windows网络编程》里面说的。
lpFlags 是in out型的，所以你不初始化是有问题的。即是传入参数，又是传出参数。
Re: neicole 2012-05-30 17:07发表 [回复]

回复OKILOVE：咦~　我测试的时候没有出现这个情况~　刚刚再试了一下，还是正常~
Re: 唐伯虎傻画画 2012-10-26 17:59发表 [回复]

回复neicole：恩 确实有这个情况，Send函数在线程里调用是无法发送数据的，我改了一下，把发送函数定义成类成员函数，然后再去调用就可以完成客户端到服务器的发送了，代码如下
class CMySend
{
public:
static void sendBuff()
{
while(1){ 
string talk; 
getline(cin, talk); 
send(sockClient, talk.c_str(), 200, 0); // 发送信息 

WaitForSingleObject(bufferMutex, INFINITE); // P（资源未被占用） 
if("quit" == talk){ 
talk.push_back('\0'); 
send(sockClient, talk.c_str(), 200, 0); 
break; 
} 
else{ 
talk.append("\n"); 
} 
printf("\nI Say:(\"quit\"to exit):"); 
cout << talk; 
ReleaseSemaphore(bufferMutex, 1, NULL); // V（资源占用完毕） 
} 
}
};

下边是线程函数调用的send函数
CMySend::sendBuff();
静态调用，就可以解决了
Re: neicole 2012-10-28 21:42发表 [回复]

回复唐伯虎傻画画：谢谢分享&&交流~不过另外说一下哦：我的原代码中没有使用类对象来调用send/recv线程函数喔，如果用上类的话，的确需要静态函数才能作为线程运行的函数的。上次1楼的朋友的问题不是由于这个静态函数的问题，而是因为他在参考代码时弄错了些参数(184行的Flags = 0不能漏)而产生的问题。
您还没有登录,请[登录]或[注册]
* 以上用户言论只代表其个人观点，不代表CSDN网站的观点或立场
个人资料
 访问我的空间 
neicole
 
访问：622952次
积分：7400
等级： 
排名：第3006名
原创：175篇转载：21篇译文：0篇评论：431条
博客简单介绍
一. 建博原因：
1. 记录大学期间学习历程
2. 方便整理归纳回顾知识
3. 受自身历程及众人的监督
4. 认识志同道合的朋友
5. 一起学习交流成长
 
[博客相关介绍点击这里]
附：我的资料库(点我)
文章分类
--> 博客总目录 <--(1)
1————————— 分界线(0)
【学习_计划_生活】(15)
【学习进度记录】每N天的学习小记(20)
【Others】其它话题或书籍(9)
2————————— 分界线(0)
【软件设计模式】(1)
【C语言】学习记录(1)
【算法竞赛入门经典】学习记录(5)
【ACM】刷题记录(2)
【编程珠玑】学习记录(1)
【QT】学习记录(9)
【Windows编程】(8)
【汇编语言】学习记录(15)
【数据结构(C++)】学习记录(22)
【数据库】学习记录(4)
【Windows网络编程】(6)
【C++ 基础类书籍】学习记录(11)
【研究课题及项目】(8)
【Windows程序设计】学习记录(3)
3————————— 分界线(0)
〖大学专业相关课程〗复习资料(4)
〖10级大一课堂习题〗C++(11)
〖10级大二课堂习题〗汇编语言(3)
〖10级大二课堂习题〗数据结构(13)
〖10级大二课堂习题〗Java(8)
〖10级大三课堂〗算法分析(3)
4————————— 分界线(0)
Windows编程(3)
C++其它习题(10)
C++知识点(6)
VLC(2)
Qt相关(2)
大学感言
        生活上，学习上的点点滴滴总是那么的神奇，过去未知答案时所走出的道路，在未来的这天，发觉还真的能将它的点点滴滴串连起来，现在所走之路，未知是何路，但必是通向未来之路~
        因为不满足于现状，觉得可以做得更好，所以，常常不断地在寻找着出路，不愿做一只井底之蛙，通过不断努力去改变现状，学习如此，生活亦是如此。不知是何时起，有这么一股想法，这么一股劲，不断寻求，相信总有一天苗子会长大成参天大树。
        假如抛开了一切外在负担，你最在乎的是什么？或者说，你最想做的事情是什么？ 燃起你的激情，为之奋斗~  梦&想，梦非仅是梦，想非仅空想，梦想并非遥不可及~  用这燃烧不尽的激情，追随着它，Achieve it ~ 不要让自己迷失方向，不要让一切邪恶的东西将其覆盖，将其浇灭。
        不要冷漠了任何一件事物，它们总是会有这么神奇的一个地方，不断地挖掘挖掘，你会懂得更多，获得更多~好奇心是充满魔力的东西~
文章搜索

搜索
阅读排行
IOCP模型与网络编程(87290)
Windows Socket 编程_ 简单的服务器/客户端程序(55553)
07_班级静态网页设计(小钱版)[Web开发][2012-11-20](21175)
libvlc_media_add_option 函数中的参数设置(17613)
我第一年的C++学习之路 —— C++学习方法不断转变(14967)
Windows Socket 编程_单个服务器对多个客户端简单通讯(12375)
VLC简介及使用说明(10340)
(2013.03.08)求最大公约数_3种算法(10023)
(2012.12.25)我的中级软件设计师考试(9513)
本地计算机网络信息的获取(6915)
文章存档
2013年08月(2)
2013年07月(1)
2013年06月(1)
2013年05月(4)
2013年03月(1)
展开
