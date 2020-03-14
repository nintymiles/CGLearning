# 简单web服务器搭建（tomcat，python http server）

有时候，当要验证一些网络相关的内容时（比如webgl），需要使用web服务器。这时候就需要简单快捷的搭建web服务器。

## 1. 使用tomcat作为临时的web服务器
有时候使用Tomcat作为临时的web服务器十分便利。跨平台而且配置安装简单。

### 关键配置
首先，在conf/server.xml中配置Host标签，将appBase设置为你的web根目录

```web.xml
<Host name="localhost"  appBase="/Users/xxx/Documents/MyWeb"
            unpackWARs="false" autoDeploy="false"> 
            
... 
```
注意，如果不设置appBase，那么tomcat默认的文档、以及默认应用可能会干扰你的web应用的二级或者更深的访问路径。

其次，在conf/server.xml中使用Context标签配置（虚拟）你的web站点目录


```web.xml
...

<Context path="" docBase="/Users/xxx/Documents/MyWeb/TWGL" debug="0" reloadable="true" />

...
```
这配置了web站点的根目录。使用 http://localhost:8080/ 访问即可。

## 2. 使用python实现随意简单的web服务器
使用python自带的http server实现其实是最简单和方便的。只要拥有python环境即可，在windows上需要安装python。由于mac/Linux系统默认自带python环境，实际上这个步骤也不需要。

### python web服务器的使用方法

#### 1. web根目录的设置
python自带的web服务器以当前目录为web的根目录，不需要设置任何复杂的配置文件，只要在命令行中cd到当前目录，然后启动python http server服务器即可。

#### 2. 启动命令
这里需要稍微注意，针对不同的python版本，启动命令稍微有些差异。在python2中，执行`python -m SimpleHTTPServer`命令，python3中，执行`python3 -m http.server`命令。

当命令执行完成后，你就可以使用http://localhost:8000进行访问了。

