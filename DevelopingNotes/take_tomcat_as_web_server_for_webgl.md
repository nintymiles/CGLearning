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

<Context path="" docBase="/Users/xxx/Documents/MyWeb/TWGL" debug="true" reloadable="true" />

...
```
这配置了web站点的根目录。使用 http://localhost:8080/ 访问即可。

## 配置的简单错误
有时候在配置xml文件时，由于网络copy或者书写问题，会导致属性无法生效，从而使得tomcat无法启动。比如，有中文字符，`<Context path="" docBase="/Users/xxx/Documents/MyWeb/TWGL" debug=“true” reloadable=“true" />`。
此时tomcat会如果出现下列信息，则预示着xml的书写错误。


```log
org.xml.sax.SAXParseException; systemId: file:/Users/xxx/Documents/TomcatServer/tomcat-7.0.99/conf/server.xml; lineNumber: 140; columnNumber: 79; 
Open quote is expected for attribute "debug" associated with an  element type  "Context".
	at com.sun.org.apache.xerces.internal.util.ErrorHandlerWrapper.createSAXParseException(ErrorHandlerWrapper.java:203)
	at com.sun.org.apache.xerces.internal.util.ErrorHandlerWrapper.fatalError(ErrorHandlerWrapper.java:177)
...
```



