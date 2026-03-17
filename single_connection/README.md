This server **only** supports single connection at a time.

By using Makefile, you may simply turn on the server and connect them witih clients.

```bash
    make
    ./server #start server
    ./client #start client
```

```bash
    make clean #delete all files except for *.cpp and Makefile
```

By the way, this server can only accecpt atmost one client a time.

For multi-client server that supports concurrent connections, the server **must** support **Multi-threading**, **Multi-processing**, or **I/O Multiplexing** (such as `select`, `poll`, or `epoll`).