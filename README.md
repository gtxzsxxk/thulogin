
# thulogin

CLI界面的贵校校园网登录器，使用c++编写，使用clion与cmake构建项目。

Usage：

```bash
./thulogin
./thulogin username
./thulogin --wifi --ethernet
./thulogin --username username [--pwd||--password] pwd
./thulogin --server url --ua user-agent --ac-id ac_id
```

最简单的使用方法：直接thulogin。如果需要配置参数，请参考usage。

贵校的ac_id分为以下两种：

- 173: 有线网络连接
- 135: wifi连接

