# 使用Tomcat作为web服务器
有时候使用Tomcat作为临时的web服务器十分便利。跨平台而且配置安装简单。

## 关键配置
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


