1.使用bat先为每个服务生成一个.h文件
将.h代码中重复的的基础数据定义注释掉。
makeproxy-wsdl2VerServH.bat
makeproxy-wsdl2TMSServH.bat

2.将多个.h文件合并为ClientServ.h中

3.根据ClientServ.h生成相关代码