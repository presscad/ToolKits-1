#pragma once

#include "stdafx.h"
#include "WxPayAPI.h"
#include "list.h"
#include "XhCharString.h"
#include "HashTable.h"
#include "Variant.h"

/**
* 	配置账号信息
*/
class CWxPayConfig
{
public:
    //=======【基本信息设置】=====================================
    /* 微信公众号信息配置
    * APPID：绑定支付的APPID（必须配置）
    * MCHID：商户号（必须配置）
    * KEY：商户支付密钥，参考开户邮件设置（必须配置）
    * APPSECRET：公众帐号secert（仅JSAPI支付的时候需要配置）
    */
    static const char* APPID;//     = "wx2e0dfd49d40a80db";
    static const char* MCHID;//     = "1493128472";
    static const char* KEY;//		= "JVAx4v73wnvt3kv3u3mtt8D8tqGH7NsY";
    static const char* APPSECRET;// = "51c56b886b5be869567dd389b3e5d1d6";

    //=======【证书路径设置】===================================== 
    /* 证书路径,注意应该填写绝对路径（仅退款、撤销订单时需要）
    */
    static const char* SSLCERT_PATH;//		= "cert/apiclient_cert.p12";
    static const char* SSLCERT_PASSWORD;//	= "1233410002";

    //=======【支付结果通知url】===================================== 
    /* 支付结果通知回调url，用于商户接收支付结果
    */
    static const char* NOTIFY_URL;

    //=======【商户系统后台机器IP】===================================== 
    /* 此参数可手动配置也可在程序中自动获取
    */
    static const char* IP;// = "8.8.8.8";


    //=======【代理服务器设置】===================================
    /* 默认IP和端口号分别为0.0.0.0和0，此时不开启代理（如有需要才设置）
    */
    static const char* PROXY_URL;	//

    //=======【上报信息配置】===================================
    /* 测速上报等级，0.关闭上报; 1.仅错误时上报; 2.全量上报
    */
    static const int REPORT_LEVENL = 1;

    //=======【日志级别】===================================
    /* 日志等级，0.不输出日志；1.只输出错误信息; 2.输出错误和正常信息; 3.输出错误信息、正常信息和调试信息
    */
    static const int LOG_LEVENL = 0;
};

/// <summary>
/// 微信支付协议接口数据类，所有的API接口通信都依赖这个数据结构，
/// 在调用接口之前先填充各个字段的值，然后进行接口通信，
/// 这样设计的好处是可扩展性强，用户可随意对协议进行更改而不用重新设计数据结构，
/// 还可以随意组合出不同的协议数据包，不用为每个协议设计一个数据包结构
/// </summary>
class CWxPayData
{
private:
    //采用排序的Dictionary的好处是方便对数据包进行签名，不用再签名之前再做一次排序
	CXhSimpleList<CXhChar100> m_keyList;	//保存有序的key
	CHashStrList<CVariant> m_values;
public:
    /**
    * 设置某个字段的值
    * @param key 字段名
    * @param value 字段值
    */
    void SetValue(const char* key, const char* data);
	void SetValue(const char* key, int data);
	void SetValue(const char* key,CVariant *pVar);

    /**
    * 根据字段名获取某个字段的值
    * @param key 字段名
    * @return key对应的字段值
    */
    CVariant* GetValue(const char* key);
	CString GetValueStr(const char* key);
    /**
    * 判断某个字段是否已设置
    * @param key 字段名
    * @return 若字段key已被设置，则返回true，否则返回false
    */
    bool IsSet(const char* key);

    /**
    * @将Dictionary转成xml
    * @return 经转换得到的xml串
    * @throws WxPayException
    **/
    CString ToXml();
    
	/**
    * @将xml转为WxPayData对象并返回对象内部的数据
    * @param string 待转换的xml串
    * @return 经转换得到的Dictionary
    * @throws WxPayException
    */
    bool FromXml(CString xml);
    
	/**
    * @Dictionary格式转化成url参数格式
    * @ return url格式串, 该串不包含sign字段值
    */
    CString ToUrl(bool bIncSign);
    
    /**
    * @Dictionary格式化成Json
    * @return json串数据
    */
    CString ToJson();
    
    /**
    * @values格式化成能在Web页面上显示的结果（因为web页面上不能直接输出xml格式的字符串）
    */
    CString ToPrintStr();
    
    /**
    * @生成签名，详见签名生成算法
    * @return 签名, sign字段不参加签名
    */
    CString MakeSign();
    
    /**
    * 
    * 检测签名是否正确
    * 正确返回true，错误抛异常
    */
    bool CheckSign();

    /**
    * @获取Dictionary
    */
    CHashStrList<CVariant>* GetValues();
};

/// <summary>
/// 回调处理基类
/// 主要负责接收微信支付后台发送过来的数据，对数据进行签名验证
/// 子类在此类基础上进行派生并重写自己的回调处理过程
/// </summary>
class CWxPayNotify
{
public:
	CWxPayNotify(){;}
	/// <summary>
	/// 接收从微信支付后台发送过来的数据并验证签名
	/// </summary>
	/// <returns>微信支付后台返回的数据</returns>
	bool GetNotifyData(CWxPayData &data);
	//派生类需要重写这个方法，进行不同的回调处理
	virtual void ProcessNotify(){;}
};

class CWxPayApi
{
public:
	/**
    * 提交被扫支付API
    * 收银员使用扫码设备读取微信用户刷卡授权码以后，二维码或条码信息传送至商户收银台，
    * 由商户收银台或者商户后台调用该接口发起支付。
    * @param WxPayData inputObj 提交给被扫支付API的参数
    * @param int timeOut 超时时间
    * @throws WxPayException
    * @return 成功时返回调用结果，其他抛异常
    */
    static bool Micropay(CWxPayData &inputObj, CWxPayData &outputObj,int timeOut = 10);
        
    /**
    *    
    * 查询订单
    * @param WxPayData inputObj 提交给查询订单API的参数
    * @param int timeOut 超时时间
    * @throws WxPayException
    * @return 成功时返回订单查询结果，其他抛异常
    */
    static bool OrderQuery(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);

    /**
    * 
    * 撤销订单API接口
    * @param WxPayData inputObj 提交给撤销订单API接口的参数，out_trade_no和transaction_id必填一个
    * @param int timeOut 接口超时时间
    * @throws WxPayException
    * @return 成功时返回API调用结果，其他抛异常
    */
    static bool Reverse(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);

    /**
    * 
    * 申请退款
    * @param WxPayData inputObj 提交给申请退款API的参数
    * @param int timeOut 超时时间
    * @throws WxPayException
    * @return 成功时返回接口调用结果，其他抛异常
    */
    static bool Refund(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);
    
    /**
	* 
	* 查询退款
	* 提交退款申请后，通过该接口查询退款状态。退款有一定延时，
	* 用零钱支付的退款20分钟内到账，银行卡支付的退款3个工作日后重新查询退款状态。
	* out_refund_no、out_trade_no、transaction_id、refund_id四个参数必填一个
	* @param WxPayData inputObj 提交给查询退款API的参数
	* @param int timeOut 接口超时时间
	* @throws WxPayException
	* @return 成功时返回，其他抛异常
	*/
	static bool RefundQuery(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);
	
    /**
    * 下载对账单
    * @param WxPayData inputObj 提交给下载对账单API的参数
    * @param int timeOut 接口超时时间
    * @throws WxPayException
    * @return 成功时返回，其他抛异常
    */
	static bool DownloadBill(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);
    
    /**
	* 
	* 转换短链接
	* 该接口主要用于扫码原生支付模式一中的二维码链接转成短链接(weixin://wxpay/s/XXXXXX)，
	* 减小二维码数据量，提升扫描速度和精确度。
	* @param WxPayData inputObj 提交给转换短连接API的参数
	* @param int timeOut 接口超时时间
	* @throws WxPayException
	* @return 成功时返回，其他抛异常
	*/
	static bool ShortUrl(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);
	
    /**
    * 
    * 统一下单
    * @param WxPaydata inputObj 提交给统一下单API的参数
    * @param int timeOut 超时时间
    * @throws WxPayException
    * @return 成功时返回，其他抛异常
    */
    static bool UnifiedOrder(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);
	
    /**
	* 
	* 关闭订单
	* @param WxPayData inputObj 提交给关闭订单API的参数
	* @param int timeOut 接口超时时间
	* @throws WxPayException
	* @return 成功时返回，其他抛异常
	*/
	static bool CloseOrder(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 6);

    /**
	* 
	* 测速上报
	* @param string interface_url 接口URL
	* @param int timeCost 接口耗时
	* @param WxPayData inputObj参数数组
	*/
    static void ReportCostTime(CString interface_url, int timeCost, CWxPayData &inputObj);


    /**
	* 
	* 测速上报接口实现
	* @param WxPayData inputObj 提交给测速上报接口的参数
	* @param int timeOut 测速上报接口超时时间
	* @throws WxPayException
	* @return 成功时返回测速上报接口返回的结果，其他抛异常
	*/
	static bool Report(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut = 1);

    /**
    * 根据当前系统时间加随机序列来生成订单号
    * @return 订单号
    */
    static CString GenerateOutTradeNo(const char* hardId,const char* applyNonceCode);
    
    /**
    * 生成时间戳，标准北京时间，时区为东八区，自1970年1月1日 0点0分0秒以来的秒数
        * @return 时间戳
    */
    static CString GenerateTimeStamp();
    /**
    * 生成随机串，随机串包含字母或数字
    * @return 随机串
    */
    static CString GenerateNonceStr();
};

//
class CNativePay
{
public:
    /**
    * 生成扫描支付模式一URL
    * @param productId 商品ID
    * @return 模式一URL
    */
    static CString GetPrePayUrl(const char* productId);
    /**
    * 生成直接支付url，支付url有效期为2小时,模式二
    * @param productId 商品ID
    * @return 模式二URL
    */
	static CString GetPayUrl(int money,const char* productId,const char* nonce_str,const char* out_trade_no,
							 const char* description,const char* attachData,const char* tag);
};