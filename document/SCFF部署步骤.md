一 安装Boost库（已安装，请忽略）
下载boost库：http://www.boost.org/
编译boost库：
1.	解压源码包
2.	./bootstrap.sh --prefix=/home/johnsonzhu/local/boost/
3.	./bjam install 或者 ./b2
二 安装fastcgi++
1.	./configure  --prefix=/home/johnsonzhu/local/fastcgipp --with-boost=/home/johnsonzhu/local/boost/
2.	make;make install;

三 安装ctemplate.0.88
1.	./configure  --prefix=/home/johnsonzhu/local/ctemplate.0.88
2.	make install

四 设置环境变量
export HOME=/home/johnsonzhu/
export FCGI_INC_EXPORT_PATH=$HOME/local/fastcgipp/include/
export BOOST_INC_EXPORT_PATH=$HOME/local/boost/include/
export CTEMPLATE_INC_EXPORT_PATH=$HOME/local/ctemplate.0.88/include/
export WEBFRAMEWORK_INC_EXPORT_PATH=$HOME/local/webframework/include/

export FCGI_LIB_EXPORT_PATH=$HOME/local/fastcgipp/lib/
export BOOST_LIB_EXPORT_PATH=$HOME/local/boost/lib/
export CTEMPLATE_LIB_EXPORT_PATH=$HOME/local/ctemplate.0.88/lib/
export WEBFRAMEWORK_LIB_EXPORT_PATH=$HOME/local/webframework/lib/

五 安装webframework
1.	make
2.	make install  （注意设置好环境变量，根据环境变量安装webframework）

六 编译Echo例子
1.	cd example/echo
2.	make

七 部署Echo例子
1.	cd webapps/echo/bin
2.	启动 ./service_manager.sh start
3.	修改Nginx配置，如
 location ~/cgi/(\w+)/ {
            fastcgi_pass unix:/dev/shm/$1.fcgi.sock;
            fastcgi_param  SCRIPT_FILENAME $document_root$fastcgi_script_name;
            include fastcgi_params;
        }
4.	访问fcgi，url如：domain.com/cgi/echo/getloc?gettest=abc


