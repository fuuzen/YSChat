# YSChat

计算机网络实验网络编程实验课程作业

基于 Winsock2 实现多用户在线聊天功能，时间有限只有 TUI 交互，较为简陋

## 开发环境
- C++20 标准
- `ncurses` 库实现 TUI 交互界面
- 套间字编程
  - Windows 平台使用 `winsock2` 库
  - UNIX 平台使用 POSIX 库
  
Windows 使用 MSYS2 安装 `ncurses` 库：

```shell shell
pacman -S mingw-w64-x86_64-ncurses
```

参考官网：https://packages.msys2.org/packages/mingw-w64-x86_64-ncurses


## 编译使用

```shell shell
cd server
g++ -std=c++20 server.cpp -lws2_32 -o server
./server.exe [filename [port1] [port2]]
# filename 指定一个文本文件，例如仓库里默认的 users.txt，每两行代表用户名和密码，作为存储用户信息的“数据库”
# port 服务端监听的端口，默认 11451

cd client
g++ -std=c++20 client.cpp -lws2_32 -o client
./server.exe [host [port1] [port2]]
# host 指定服务端运行的主机名或 ip
# port 服务端监听的端口，默认 11451
```

注意  Windows 需要使用 GBK 编码打开和保存源文件，才能保证编译出来的程序能输出中文