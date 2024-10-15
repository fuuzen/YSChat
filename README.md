# YSChat

计算机网络实验网络编程实验课程作业

基于 Winsock2 实现多用户在线聊天功能，时间有限只有 CLI 交互，较为简陋

编译使用：

```shell shell

cd server
g++ -std=c++20 server.cpp -lws2_32 -o server
./server.exe [filename [port1] [port2]]
# filename 指定一个文本文件，例如仓库里默认的 users.txt，每两行代表用户名和密码，作为存储用户信息的“数据库”
# port1 客户端从这个指定端口传输信息，包括登录信息，默认 11451
# port2 客户端从这个指定端口接受信息，默认 14514

cd client
g++ -std=c++20 client.cpp -lws2_32 -o client
./server.exe [host [port1] [port2]]
# host 指定服务端运行的主机名或 ip
# port1 port2 同上
```

注意  Windows 需要使用 GBK 编码打开和保存源文件，才能保证编译出来的程序能输出中文