# MultiCabinet
1. Implement a versatile comprehensive service cabinet capable of handling multiple concurrent and parallel functionalities for centralized management of services such as food delivery cabinets, parcel lockers, and vending machines using sockets, multiprocessing, multithreading, semaphores, and pipes.

2. Establish a connection between the server and clients using sockets. Clients send request data packets, including command codes, user IDs, and request data. For parcel lockers and food delivery cabinets, the request data is the retrieval code, and for vending machines, it is the product code.

3. The server creates three child processes, each responsible for handling data requests for parcel lockers, food delivery cabinets, and vending machines. After accepting a client connection, the server allocates space to store the client handle, then creates a child thread and passes the client handle pointer. Thus, for each client connection, a child thread is created to handle data reception. In the child thread, data packets are categorized based on command codes. The relevant process is awakened using semaphores, and data is forwarded through pipes. The child thread then waits for feedback semaphores and feedback data.

4. Each child process continuously waits for semaphores. Upon receiving a semaphore, it creates a child thread to handle data. In the child thread of each child process, data is received through pipes, processed, and feedback semaphores and data are returned. The main process child thread receives the feedback semaphores and feedback data and sends them back to the client.

5. The client receives the data and displays the operation results.


# 多功能综合服务柜

1. 通过套接字、多进程、多线程、信号量、管道实现能够承受多并发多并行的多功能综合服务柜，对外卖柜、快递柜、售货柜等进行集中管理；
2. 通过套接字建立服务器与客户端之间的联系，客户端发送请求数据包，数据内容包括命令代码、用户ID以及请求数据，请求数据对于快递柜及外卖柜而言为取件码，对于售货柜而言为商品编号；
3. 服务器端创建三个子进程，分别对应处理快递柜、外卖柜及售货柜的数据请求，在接受客户端连接后申请空间存放客户端句柄，随后创建子线程并将客户端句柄指针传入，即每接受一个客户端连接，创建一个子线程负责处理数据接收，在子线程中根据数据包命令代码分类，通过信号量唤醒指定进程，并通过管道转发数据，然后阻塞等待反馈信号量及反馈数据；
4. 每一个子进程都处于循环等待信号量，每次接收信号量之后创建子线程处理数据，在子进程的子线程中通过管道接收数据，处理完成后返回反馈信号量及反馈数据，主进程子线程获得反馈信号量及反馈数据后，再次发送回客户端；
5. 客户端接收数据显示操作结果。

![结构框图](https://github.com/Olaaaaab/MultiCabinet/assets/139237529/af4e7d2f-0996-4ea6-84a5-4d47b1951eb7)
