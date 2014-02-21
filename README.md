SCFF
====

Simple C++ FastCgi Framework 简单便捷的c++ FastCgi 框架，快速生成代码框架，关注业务CGI快速开发。对接各种提供fastcgi协议的服务器。

====

SCFF简介
一 SCFF是基于FastCgi协议的C++ Web框架

二 SCFF提供了代码框架AutoGen

通过echo.h文件autogen出代码框架，lib提供接口路由及参数封装功能，只需要在
Src目录实现业务功能接口即可。

三 SCFF提供执行框架及工具

把编译好的fcgi放到cgi目录，并且修改配置文件及bin目录的service_manage.sh和
protect.sh 。
通过service_manage.sh进行管理进程启动和关闭。
通过protect来守护fcgi进程正常服务及告警。

四 SCFF提供usocket方式提供FastCgi服务，可通过Nginx路由多个服务。
 location ~/cgi/(\w+)/ {
            fastcgi_pass unix:/dev/shm/$1.fcgi.sock;
            fastcgi_param  SCRIPT_FILENAME $document_root$fastcgi_script_name;
            include fastcgi_params;
        }

五 SCFF高性能FastCgi服务。
性能压测：(4核、8GMEM)单机HelloWord达到4.3k/s，（空转nginx是4.6k/s）。贴近nginx的处理能力。
