#include "stdafx.h"
#include "WxPayAPI.h"
#include "Markup.h"
#include "MD5.h"
#include "ArrayList.h"
#include "HttpClient.h"
#include "SortFunc.h"

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
/* 微信公众号信息配置
* APPID：绑定支付的APPID（必须配置）
* MCHID：商户号（必须配置）
* KEY：商户支付密钥，参考开户邮件设置（必须配置）
* APPSECRET：公众帐号secert（仅JSAPI支付的时候需要配置）
*/
const char* CWxPayConfig::APPID		= "wx2e0dfd49d40a80db";
const char* CWxPayConfig::MCHID		= "1493128472";
const char* CWxPayConfig::KEY		= "JVAx4v73wnvt3kv3u3mtt8D8tqGH7NsY";
const char* CWxPayConfig::APPSECRET	= "51c56b886b5be869567dd389b3e5d1d6";

//=======【证书路径设置】===================================== 
/* 证书路径,注意应该填写绝对路径（仅退款、撤销订单时需要）
*/
const char* CWxPayConfig::SSLCERT_PATH		= "cert/apiclient_cert.p12";
const char* CWxPayConfig::SSLCERT_PASSWORD	= "1233410002";

//=======【支付结果通知url】===================================== 
/* 支付结果通知回调url，用于商户接收支付结果
*/
const char* CWxPayConfig::NOTIFY_URL	= "http://www.xerofox.com/PayNotify.aspx";

//=======【商户系统后台机器IP】===================================== 
/* 此参数可手动配置也可在程序中自动获取
*/
const char* CWxPayConfig::IP = "8.8.8.8";


//=======【代理服务器设置】===================================
/* 默认IP和端口号分别为0.0.0.0和0，此时不开启代理（如有需要才设置）
*/
const char* CWxPayConfig::PROXY_URL	= "";

//////////////////////////////////////////////////////////////////////////
// CWxPayData
//////////////////////////////////////////////////////////////////////////
void CWxPayData::SetValue(const char* key,const char* data)
{
	m_keyList.AttachNode(CXhChar100(key));
	CVariant var;
	var.SetGridString(data);
    m_values.SetValue(key,var);
}

void CWxPayData::SetValue(const char* key, int data)
{
	m_keyList.AttachNode(CXhChar100(key));
	CVariant var;
	var.vType=GRID_VT_I4;
	var.lVal=data;
	m_values.SetValue(key,var);
}

void CWxPayData::SetValue(const char* key,CVariant *pVar)
{
	if(pVar==NULL)
		return;
	m_keyList.AttachNode(CXhChar100(key));
	m_values.SetValue(key,*pVar);
}

CVariant* CWxPayData::GetValue(const char* key)
{
    return m_values.GetValue(key);
}

CString CWxPayData::GetValueStr(const char* key)
{
	CVariant *pVar=GetValue(key);
	if(pVar==NULL)
		return CString();
	CString str;
	if(pVar->vType==GRID_VT_STRING)
		str.Format(pVar->sVal);
	else if(pVar->vType==GRID_VT_I4)
		str.Format("%d",pVar->lVal);
	return str;
}

bool CWxPayData::IsSet(const char* key)
{
    if(m_values.GetValue(key)==NULL)
		return false;
	else
		return true;
}

static int compare_keystr(const CXhChar100& item1,const CXhChar100& item2)
{
	CXhChar100 str1(item1),str2(item2);
	int len1=str1.GetLength();
	int len2=str2.GetLength();
	int i = 0;  
	while(i<len1&&i<len2)
	{
		if(str1.At(i)>str2.At(i))
			return 1;
		else if(str1.At(i)<str2.At(i))
			return -1;
		i++;
	}
	if(len1<len2)
		return -1;
	else if(len1>len2)
		return 1;
	else
		return 0;
}

CString CWxPayData::ToXml()
{
    CString sXML;
	//数据为空时不能转化为xml格式
    if (0 == m_values.GetNodeNum())
        return sXML;
	ARRAY_LIST<CXhChar100> keyArr;
	for(CXhChar100 *pKey=m_keyList.EnumObjectFirst();pKey;pKey=m_keyList.EnumObjectNext())
		keyArr.append(*pKey);
	CHeapSort<CXhChar100>::HeapSort(keyArr.m_pData,keyArr.Size(),compare_keystr);
	sXML.Append("<xml>");
	for(CXhChar100 *pKey=keyArr.GetFirst();pKey;pKey=keyArr.GetNext())
    {
        //字段值不能为null，会影响后续流程
		CVariant *pVar=m_values.GetValue(*pKey);
        if (pVar==NULL)
            return CString();
        if (pVar->vType == GRID_VT_I4)
			sXML.AppendFormat("<%s>%d</%s>",(char*)(*pKey),pVar->lVal,(char*)(*pKey));
        else if (pVar->vType==GRID_VT_STRING)
			sXML.AppendFormat("<%s><![CDATA[%s]]></%s>",(char*)(*pKey),pVar->sVal,(char*)(*pKey));
        else//除了CString和int类型不能含有其他数据类型
            return CString();
    }
    sXML.Append("</xml>");
    return sXML;
}

bool CWxPayData::FromXml(CString xml)
{
    if (xml.GetLength()==0)
    {
        //Log.Error(this.GetType().ToString(), "将空的xml串转换为WxPayData不合法!");
        throw CString("将空的xml串转换为WxPayData不合法!");
		return false;
    }
	CMarkup xmlDoc(xml); 
    xmlDoc.ResetMainPos();
	CString sValue=_T("");
	while(xmlDoc.FindChildElem())   
	{
		xmlDoc.IntoElem();
		CString sTag=xmlDoc.GetTagName();
		CString sData=xmlDoc.GetData();
		SetValue(sTag,sData);
		xmlDoc.OutOfElem();
	}		
    try
    {
		//2015-06-29 错误是没有签名
		CVariant *pVar=m_values.GetValue("return_code");
		if(pVar==NULL || pVar->vType!=GRID_VT_STRING || strcmp(pVar->sVal,"SUCCESS")!=0)
		{
			return false;
		}
        CheckSign();//验证签名,不通过会抛异常
    }
    catch(CString sError)
    {
        throw sError;
    }
    return true;
}

CString CWxPayData::ToUrl(bool bIncSign)
{
    CString buff = "";
	if(m_keyList.Count<=0)
		return CString();
	ARRAY_LIST<CXhChar100> keyArr;
	for(CXhChar100 *pKey=m_keyList.EnumObjectFirst();pKey;pKey=m_keyList.EnumObjectNext())
		keyArr.append(*pKey);
	CHeapSort<CXhChar100>::HeapSort(keyArr.m_pData,keyArr.Size(),compare_keystr);
	for(CXhChar100 *pKey=keyArr.GetFirst();pKey;pKey=keyArr.GetNext())
	{	//字段值不能为null，会影响后续流程
		CVariant *pVar=m_values.GetValue(*pKey);
		if (pVar==NULL)
		{
			throw CString("WxPayData内部含有值为null的字段!");
			return CString();
		}
		if ((bIncSign||!pKey->EqualNoCase("sign")) && ((pVar->vType==GRID_VT_STRING && strlen(pVar->sVal)>0)||pVar->vType==GRID_VT_I4))
		{
			if(buff.GetLength()>0)
				buff.Append("&");
			if(pVar->vType==GRID_VT_STRING)
				buff.AppendFormat("%s=%s",(char*)(*pKey),(char*)pVar->sVal);
			else
				buff.AppendFormat("%s=%d",(char*)(*pKey),pVar->lVal);
		}
	}
    return buff;
}

CString CWxPayData::ToJson()
{
    CString jsonStr;// = JsonMapper.ToJson(m_values);
    return jsonStr;
}

CString CWxPayData::ToPrintStr()
{
    CString str = "";
	for(CXhChar100 *pKey=m_keyList.EnumObjectFirst();pKey;pKey=m_keyList.EnumObjectNext())
	{	//字段值不能为null，会影响后续流程
		CVariant *pVar=m_values.GetValue(*pKey);
		if (pVar==NULL)
		{
			throw CString("WxPayData内部含有值为null的字段!");
			return CString();
		}
		if(pVar->vType==GRID_VT_STRING)
			str.AppendFormat("%s=%s<br>",(char*)(*pKey),(char*)pVar->sVal);
		else
			str.AppendFormat("%s=%d<br>",(char*)(*pKey),pVar->lVal);
	}
    return str;
}
static BOOL StringToMemory(const char* str,BYTE* buf,UINT buf_length/*=16*/)
{
	UINT str_length=buf_length*2;
	if(strlen(str)<str_length)
	{
		memset(buf,0,buf_length);
		return FALSE;
	}
	for(UINT i=0;i<str_length;i+=2)
	{
		BYTE low=0,high=0;
		if(str[i]>='0'&&str[i]<='9')
			low=str[i]-'0';
		else if(str[i]>='A'&&str[i]<='F')
			low=str[i]-'A'+10;
		else if(str[i]>='a'&&str[i]<='f')
			low=str[i]-'a'+10;
		if(str[i+1]>='0'&&str[i+1]<='9')
			high=str[i+1]-'0';
		else if(str[i+1]>='A'&&str[i+1]<='F')
			high=str[i+1]-'A'+10;
		else if(str[i+1]>='a'&&str[i+1]<='f')
			high=str[i+1]-'a'+10;
		high<<=4;
		buf[i/2]=low+high;
	}
	return TRUE;
}
static BOOL MemoryToString(const BYTE* buf,char* str,UINT buf_length/*=16*/)
{
	UINT str_length=buf_length*2;
	for(UINT i=0;i<buf_length;i++)
	{
		BYTE low=buf[i]&0x0f;
		BYTE high=(buf[i]>>4)&0x0f;
		if(low<10)
			str[i*2]='0'+low;
		else
			str[i*2]='A'+low-10;
		if(high<10)
			str[i*2+1]='0'+high;
		else
			str[i*2+1]='A'+high-10;
	}
	return TRUE;
}

CString GBK2UTF8(const CString& strGBK)
{
	wchar_t* str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, nullptr, 0);
	str1 = new wchar_t[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK, -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, nullptr, 0, nullptr, nullptr);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, nullptr, nullptr);
	CString strUTF8(str2);
	delete[]str1;
	delete[]str2;
	return strUTF8;
}

CString CWxPayData::MakeSign()
{
    //转url格式
    CString str = ToUrl(false);
    //在CString后加入API KEY
    str.AppendFormat("&key=%s",CWxPayConfig::KEY);
	//请求str内容先转为UTF-8编码
	CString ss=GBK2UTF8(str);
	//MD5加密
	CString sMD5=MD5Str((BYTE*)ss.GetBuffer(),ss.GetLength());
	/*CString sSrc="appid=wx2428e34e0e7dc6ef&attach=test&body=TMA&goods_tag=test&key=e10adc3849ba56abbe56e056f20f883e&mch_id=1233410002&nonce_str=10B93C9C7871467F9A4BDDE164EFEEFB&notify_url=http://paysdk.weixin.qq.com/example/ResultNotifyPage.aspx&out_trade_no=2636002815101189630&product_id=123456789&spbill_create_ip=8.8.8.8&time_expire=20171108133923&time_start=20171108132923&total_fee=1&trade_type=NATIVE&key=e10adc3849ba56abbe56e056f20f883e";
	sMD5=MD5Str((BYTE*)sSrc.GetBuffer(),sSrc.GetLength());
	sSrc="appid=wx2428e34e0e7dc6ef&attach=test&body=TMA&goods_tag=test&key=e10adc3849ba56abbe56e056f20f883e&mch_id=1233410002&nonce_str=10B93C9C7871467F9A4BDDE164EFEEFB&notify_url=http://paysdk.weixin.qq.com/example/ResultNotifyPage.aspx&out_trade_no=2636002815101189630&product_id=123456789&spbill_create_ip=8.8.8.8&time_expire=20171108133923&time_start=20171108132923&total_fee=1&trade_type=NATIVE&key=e10adc3849ba56abbe56e056f20f883e";
	CString sM=MD5Str((BYTE*)sSrc.GetBuffer(),sSrc.GetLength());*/
	//所有字符转为大写
    return sMD5.MakeUpper();
}

bool CWxPayData::CheckSign()
{
    //如果没有设置签名，则跳过检测
    if (!IsSet("sign"))
    {
        throw CString("WxPayData签名存在但不合法!");
    }
	CVariant *pVar=GetValue("sign");
    //如果设置了签名但是签名为空，则抛异常
    if(pVar==NULL||pVar->vType!=GRID_VT_STRING||strlen(pVar->sVal)==0)
    {
        throw CString("WxPayData签名存在但不合法!");
    }

    //获取接收到的签名
    CString return_sign = pVar->sVal;
    //在本地计算新的签名
    CString cal_sign = MakeSign();
    if (cal_sign == return_sign)
    {
        return true;
    }
    throw CString("WxPayData签名验证错误!");
}

CHashStrList<CVariant>* CWxPayData::GetValues()
{
    return &m_values;
}

//////////////////////////////////////////////////////////////////////////
// CWxPayNotify
//////////////////////////////////////////////////////////////////////////
bool CWxPayNotify::GetNotifyData(CWxPayData &data)
{
	//接收从微信后台POST过来的数据
	/*System.IO.Stream s = page.Request.InputStream;
	int count = 0;
	byte[] buffer = new byte[1024];
	StringBuilder builder = new StringBuilder();
	while ((count = s.Read(buffer, 0, 1024)) > 0)
	{
		builder.Append(Encoding.UTF8.GetString(buffer, 0, count));
	}
	s.Flush();
	s.Close();
	s.Dispose();

	Log.Info(this.GetType().ToString(), "Receive data from WeChat : " + builder.ToString());

	//转换数据格式并验证签名
	WxPayData data = new WxPayData();
	try
	{
		data.FromXml(builder.ToString());
	}
	catch(WxPayException ex)
	{
		//若签名错误，则立即返回结果给微信支付后台
		WxPayData res = new WxPayData();
		res.SetValue("return_code", "FAIL");
		res.SetValue("return_msg", ex.Message);
		Log.Error(this.GetType().ToString(), "Sign check error : " + res.ToXml());
		page.Response.Write(res.ToXml());
		page.Response.End();
	}

	Log.Info(this.GetType().ToString(), "Check sign success");*/
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CWxPayApi
//////////////////////////////////////////////////////////////////////////
bool CWxPayApi::Micropay(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 10*/)
{
    CString url = "https://api.mch.weixin.qq.com/pay/micropay";
    //检测必填参数
    if (!inputObj.IsSet("body"))
    {
        throw CString("提交被扫支付API接口中，缺少必填参数body！");
    }
    else if (!inputObj.IsSet("out_trade_no"))
    {
        throw CString("提交被扫支付API接口中，缺少必填参数out_trade_no！");
    }
    else if (!inputObj.IsSet("total_fee"))
    {
        throw CString("提交被扫支付API接口中，缺少必填参数total_fee！");
    }
    else if (!inputObj.IsSet("auth_code"))
    {
        throw CString("提交被扫支付API接口中，缺少必填参数auth_code！");
    }
       
    inputObj.SetValue("spbill_create_ip", CWxPayConfig::IP);//终端ip
    inputObj.SetValue("appid", CWxPayConfig::APPID);//公众账号ID
    inputObj.SetValue("mch_id", CWxPayConfig::MCHID);//商户号
    inputObj.SetValue("nonce_str", GenerateNonceStr());//随机字符串
    inputObj.SetValue("sign", inputObj.MakeSign());//签名
    CString xml = inputObj.ToXml();

	DWORD start=GetTickCount();	//请求开始时间

    //Log.Debug("WxPayApi", "MicroPay request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);//调用HTTP通信接口以提交数据到API
    //Log.Debug("WxPayApi", "MicroPay response : " + response);

	DWORD end = GetTickCount();
    DWORD timeCost = end-start;//获得接口耗时

    //将xml格式的结果转换为对象以返回
    outputObj.FromXml(response);
    ReportCostTime(url, timeCost, outputObj);//测速上报

    return true;
}

        
/**
*    
* 查询订单
* @param WxPayData inputObj 提交给查询订单API的参数
* @param int timeOut 超时时间
* @throws WxPayException
* @return 成功时返回订单查询结果，其他抛异常
*/
bool CWxPayApi::OrderQuery(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
    CString url = "https://api.mch.weixin.qq.com/pay/orderquery";
    //检测必填参数
    if (!inputObj.IsSet("out_trade_no") && !inputObj.IsSet("transaction_id"))
    {
        throw CString("订单查询接口中，out_trade_no、transaction_id至少填一个！");
    }

    inputObj.SetValue("appid", CWxPayConfig::APPID);//公众账号ID
    inputObj.SetValue("mch_id", CWxPayConfig::MCHID);//商户号
    inputObj.SetValue("nonce_str", CWxPayApi::GenerateNonceStr());//随机字符串
    inputObj.SetValue("sign", inputObj.MakeSign());//签名

    CString xml = inputObj.ToXml();

    DWORD start = GetTickCount();

    //Log.Debug("WxPayApi", "OrderQuery request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);//调用HTTP通信接口提交数据
    //Log.Debug("WxPayApi", "OrderQuery response : " + response);

	DWORD end = GetTickCount();
    int timeCost = end-start;//获得接口耗时

    //将xml格式的数据转化为对象以返回
    outputObj.FromXml(response);
    ReportCostTime(url, timeCost, outputObj);//测速上报

    return true;
}


/**
* 
* 撤销订单API接口
* @param WxPayData inputObj 提交给撤销订单API接口的参数，out_trade_no和transaction_id必填一个
* @param int timeOut 接口超时时间
* @throws WxPayException
* @return 成功时返回API调用结果，其他抛异常
*/
bool CWxPayApi::Reverse(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
    CString url = "https://api.mch.weixin.qq.com/secapi/pay/reverse";
    //检测必填参数
    if (!inputObj.IsSet("out_trade_no") && !inputObj.IsSet("transaction_id"))
    {
        throw CString("撤销订单API接口中，参数out_trade_no和transaction_id必须填写一个！");
    }

    inputObj.SetValue("appid", CWxPayConfig::APPID);//公众账号ID
    inputObj.SetValue("mch_id", CWxPayConfig::MCHID);//商户号
    inputObj.SetValue("nonce_str", GenerateNonceStr());//随机字符串
    inputObj.SetValue("sign", inputObj.MakeSign());//签名
    CString xml = inputObj.ToXml();

    DWORD start = GetTickCount();//请求开始时间

    //Log.Debug("WxPayApi", "Reverse request : " + xml);

    CString response = CHttpClient::Post(xml, url, true, timeOut);

    //Log.Debug("WxPayApi", "Reverse response : " + response);

    DWORD end = GetTickCount();
    int timeCost = end-start;

    outputObj.FromXml(response);

    ReportCostTime(url, timeCost, outputObj);//测速上报

    return true;
}


/**
* 
* 申请退款
* @param WxPayData inputObj 提交给申请退款API的参数
* @param int timeOut 超时时间
* @throws WxPayException
* @return 成功时返回接口调用结果，其他抛异常
*/
bool CWxPayApi::Refund(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
    CString url = "https://api.mch.weixin.qq.com/secapi/pay/refund";
    //检测必填参数
    if (!inputObj.IsSet("out_trade_no") && !inputObj.IsSet("transaction_id"))
    {
        throw CString("退款申请接口中，out_trade_no、transaction_id至少填一个！");
    }
    else if (!inputObj.IsSet("out_refund_no"))
    {
        throw CString("退款申请接口中，缺少必填参数out_refund_no！");
    }
    else if (!inputObj.IsSet("total_fee"))
    {
        throw CString("退款申请接口中，缺少必填参数total_fee！");
    }
    else if (!inputObj.IsSet("refund_fee"))
    {
        throw CString("退款申请接口中，缺少必填参数refund_fee！");
    }
    else if (!inputObj.IsSet("op_user_id"))
    {
        throw CString("退款申请接口中，缺少必填参数op_user_id！");
    }

    inputObj.SetValue("appid", CWxPayConfig::APPID);//公众账号ID
    inputObj.SetValue("mch_id", CWxPayConfig::MCHID);//商户号
    inputObj.SetValue("nonce_str", GenerateNonceStr());//随机字符串
    inputObj.SetValue("sign", inputObj.MakeSign());//签名
            
    CString xml = inputObj.ToXml();
    DWORD start = GetTickCount();

    //Log.Debug("WxPayApi", "Refund request : " + xml);
    CString response = CHttpClient::Post(xml, url, true, timeOut);//调用HTTP通信接口提交数据到API
    //Log.Debug("WxPayApi", "Refund response : " + response);

    DWORD end = GetTickCount();
    int timeCost = end-start;//获得接口耗时

    //将xml格式的结果转换为对象以返回
    outputObj.FromXml(response);
    ReportCostTime(url, timeCost, outputObj);//测速上报

    return true;
}


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
bool CWxPayApi::RefundQuery(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
	CString url = "https://api.mch.weixin.qq.com/pay/refundquery";
	//检测必填参数
	if(!inputObj.IsSet("out_refund_no") && !inputObj.IsSet("out_trade_no") &&
		!inputObj.IsSet("transaction_id") && !inputObj.IsSet("refund_id"))
    {
		throw CString("退款查询接口中，out_refund_no、out_trade_no、transaction_id、refund_id四个参数必填一个！");
	}

	inputObj.SetValue("appid",CWxPayConfig::APPID);//公众账号ID
	inputObj.SetValue("mch_id",CWxPayConfig::MCHID);//商户号
	inputObj.SetValue("nonce_str",GenerateNonceStr());//随机字符串
	inputObj.SetValue("sign",inputObj.MakeSign());//签名

	CString xml = inputObj.ToXml();
		
	DWORD start = GetTickCount();//请求开始时间

    //Log.Debug("WxPayApi", "RefundQuery request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);//调用HTTP通信接口以提交数据到API
    //Log.Debug("WxPayApi", "RefundQuery response : " + response);

    DWORD end = GetTickCount();
    int timeCost = end-start;//获得接口耗时

    //将xml格式的结果转换为对象以返回
    outputObj.FromXml(response);

	ReportCostTime(url, timeCost, outputObj);//测速上报
		
	return true;
}


/**
* 下载对账单
* @param WxPayData inputObj 提交给下载对账单API的参数
* @param int timeOut 接口超时时间
* @throws WxPayException
* @return 成功时返回，其他抛异常
*/
bool CWxPayApi::DownloadBill(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
    CString url = "https://api.mch.weixin.qq.com/pay/downloadbill";
    //检测必填参数
    if (!inputObj.IsSet("bill_date"))
    {
        throw CString("对账单接口中，缺少必填参数bill_date！");
    }

    inputObj.SetValue("appid", CWxPayConfig::APPID);//公众账号ID
    inputObj.SetValue("mch_id", CWxPayConfig::MCHID);//商户号
    inputObj.SetValue("nonce_str", GenerateNonceStr());//随机字符串
    inputObj.SetValue("sign", inputObj.MakeSign());//签名

    CString xml = inputObj.ToXml();

    //Log.Debug("WxPayApi", "DownloadBill request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);//调用HTTP通信接口以提交数据到API
    //Log.Debug("WxPayApi", "DownloadBill result : " + response);

    //若接口调用失败会返回xml格式的结果
	CString head=response.Mid(0,5);
    if (head.CompareNoCase("<xml>")==0)
    {
        outputObj.FromXml(response);
    }
    //接口调用成功则返回非xml格式的数据
    else
        outputObj.SetValue("result", response);

    return true;
}


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
bool CWxPayApi::ShortUrl(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
	CString url = "https://api.mch.weixin.qq.com/tools/shorturl";
	//检测必填参数
	if(!inputObj.IsSet("long_url"))
    {
		throw CString("需要转换的URL，签名用原串，传输需URL encode！");
	}

	inputObj.SetValue("appid",CWxPayConfig::APPID);//公众账号ID
	inputObj.SetValue("mch_id",CWxPayConfig::MCHID);//商户号
	inputObj.SetValue("nonce_str",GenerateNonceStr());//随机字符串	
	inputObj.SetValue("sign",inputObj.MakeSign());//签名
	CString xml = inputObj.ToXml();
		
	DWORD start = GetTickCount();//请求开始时间

    //Log.Debug("WxPayApi", "ShortUrl request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);
    //Log.Debug("WxPayApi", "ShortUrl response : " + response);

    DWORD end = GetTickCount();
    int timeCost = end-start;

    outputObj.FromXml(response);
	ReportCostTime(url, timeCost, outputObj);//测速上报
		
	return true;
}


/**
* 
* 统一下单
* @param WxPaydata inputObj 提交给统一下单API的参数
* @param int timeOut 超时时间
* @throws WxPayException
* @return 成功时返回，其他抛异常
*/
bool CWxPayApi::UnifiedOrder(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
    CString url = "https://api.mch.weixin.qq.com/pay/unifiedorder";
    //检测必填参数
    if (!inputObj.IsSet("out_trade_no"))
    {
        throw CString("缺少统一支付接口必填参数out_trade_no！");
    }
    else if (!inputObj.IsSet("body"))
    {
        throw CString("缺少统一支付接口必填参数body！");
    }
    else if (!inputObj.IsSet("total_fee"))
    {
        throw CString("缺少统一支付接口必填参数total_fee！");
    }
    else if (!inputObj.IsSet("trade_type"))
    {
        throw CString("缺少统一支付接口必填参数trade_type！");
    }

    //关联参数
    if (inputObj.GetValueStr("trade_type").CompareNoCase("JSAPI")==0 && !inputObj.IsSet("openid"))
    {
        throw CString("统一支付接口中，缺少必填参数openid！trade_type为JSAPI时，openid为必填参数！");
    }
    if (inputObj.GetValueStr("trade_type").CompareNoCase("NATIVE")==0 && !inputObj.IsSet("product_id"))
    {
        throw CString("统一支付接口中，缺少必填参数product_id！trade_type为JSAPI时，product_id为必填参数！");
    }
    //签名
    inputObj.SetValue("sign", inputObj.MakeSign());
    CString xml = inputObj.ToXml();

    DWORD start = GetTickCount();

    //Log.Debug("WxPayApi", "UnfiedOrder request : " + xml);
    CString response = CHttpClient::Post(xml, url, false, timeOut);
    //Log.Debug("WxPayApi", "UnfiedOrder response : " + response);

    DWORD end = GetTickCount();
    int timeCost = end-start;
    outputObj.FromXml(response);

    ReportCostTime(url, timeCost, outputObj);//测速上报

    return true;
}

 
/**
* 
* 关闭订单
* @param WxPayData inputObj 提交给关闭订单API的参数
* @param int timeOut 接口超时时间
* @throws WxPayException
* @return 成功时返回，其他抛异常
*/
bool CWxPayApi::CloseOrder(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 6*/)
{
	CString url = "https://api.mch.weixin.qq.com/pay/closeorder";
	//检测必填参数
	if(!inputObj.IsSet("out_trade_no"))
    {
		throw CString("关闭订单接口中，out_trade_no必填！");
	}

	inputObj.SetValue("appid",CWxPayConfig::APPID);//公众账号ID
	inputObj.SetValue("mch_id",CWxPayConfig::MCHID);//商户号
	inputObj.SetValue("nonce_str",GenerateNonceStr());//随机字符串		
	inputObj.SetValue("sign",inputObj.MakeSign());//签名
	CString xml = inputObj.ToXml();
		
	DWORD start = GetTickCount();//请求开始时间

    CString response = CHttpClient::Post(xml, url, false, timeOut);

    DWORD end = GetTickCount();
    int timeCost = end-start;
    outputObj.FromXml(response);

	ReportCostTime(url, timeCost, outputObj);//测速上报
		
	return true;
}


/**
* 
* 测速上报
* @param CString interface_url 接口URL
* @param int timeCost 接口耗时
* @param WxPayData inputObj参数数组
*/
void CWxPayApi::ReportCostTime(CString interface_url, int timeCost, CWxPayData &inputObj)
{
	//错误代码
	if(inputObj.IsSet("err_code")&&inputObj.IsSet("err_code_des"))
	{
		CString ss=inputObj.GetValueStr("err_code");
		ss+=inputObj.GetValueStr("err_code_des");
		AfxMessageBox(ss);
		return;
	}
	//如果不需要进行上报
	if(CWxPayConfig::REPORT_LEVENL == 0)
    {
		return;
	} 

	//如果仅失败上报
	if(CWxPayConfig::REPORT_LEVENL == 1 && inputObj.IsSet("return_code") && inputObj.GetValueStr("return_code").CompareNoCase("SUCCESS")==0 &&
		inputObj.IsSet("result_code") && inputObj.GetValueStr("result_code").CompareNoCase("SUCCESS")==0)
    {
		return;
	}
		 
	//上报逻辑
	CWxPayData data;
    data.SetValue("interface_url",interface_url);
	data.SetValue("execute_time_",timeCost);
	//返回状态码
	if(inputObj.IsSet("return_code"))
    {
		data.SetValue("return_code",inputObj.GetValue("return_code"));
	}
	//返回信息
    if(inputObj.IsSet("return_msg"))
    {
		data.SetValue("return_msg",inputObj.GetValue("return_msg"));
	}
	//业务结果
    if(inputObj.IsSet("result_code"))
    {
		data.SetValue("result_code",inputObj.GetValue("result_code"));
	}
	//错误代码
    if(inputObj.IsSet("err_code"))
    {
		data.SetValue("err_code",inputObj.GetValue("err_code"));
	}
	//错误代码描述
    if(inputObj.IsSet("err_code_des"))
    {
		data.SetValue("err_code_des",inputObj.GetValue("err_code_des"));
	}
	//商户订单号
    if(inputObj.IsSet("out_trade_no"))
    {
		data.SetValue("out_trade_no",inputObj.GetValue("out_trade_no"));
	}
	//设备号
    if(inputObj.IsSet("device_info"))
    {
		data.SetValue("device_info",inputObj.GetValue("device_info"));
	}
		
	try
    {
		CWxPayData outputData;
		Report(data,outputData);
	}
    catch (CString ex)
    {	//不做任何处理
		CString sMessage=inputObj.GetValueStr("return_msg");
		if(sMessage.GetLength()>0)
		{
			CString ss=sMessage+ex;
			AfxMessageBox(ss);
		}
		else
			AfxMessageBox(ex);
	}
}


/**
* 
* 测速上报接口实现
* @param WxPayData inputObj 提交给测速上报接口的参数
* @param int timeOut 测速上报接口超时时间
* @throws WxPayException
* @return 成功时返回测速上报接口返回的结果，其他抛异常
*/
bool CWxPayApi::Report(CWxPayData &inputObj, CWxPayData &outputObj, int timeOut/* = 1*/)
{
	CString url = "https://api.mch.weixin.qq.com/payitil/report";
	//检测必填参数
	if(!inputObj.IsSet("interface_url"))
    {
		throw CString("接口URL，缺少必填参数interface_url！");
	} 
    if(!inputObj.IsSet("return_code"))
    {
		throw CString("返回状态码，缺少必填参数return_code！");
	} 
    if(!inputObj.IsSet("result_code"))
    {
		throw CString("业务结果，缺少必填参数result_code！");
	} 
    if(!inputObj.IsSet("user_ip"))
    {
		throw CString("访问接口IP，缺少必填参数user_ip！");
	} 
    if(!inputObj.IsSet("execute_time_"))
    {
		throw CString("接口耗时，缺少必填参数execute_time_！");
	}

	inputObj.SetValue("appid",CWxPayConfig::APPID);//公众账号ID
	inputObj.SetValue("mch_id",CWxPayConfig::MCHID);//商户号
	inputObj.SetValue("user_ip",CWxPayConfig::IP);//终端ip
	CTime t=CTime::GetCurrentTime();
	inputObj.SetValue("time",t.Format("%Y%m%d%H%M%S"));//商户上报时间	 
	inputObj.SetValue("nonce_str",GenerateNonceStr());//随机字符串
	inputObj.SetValue("sign",inputObj.MakeSign());//签名
	CString xml = inputObj.ToXml();

    //Log.Info("WxPayApi", "Report request : " + xml);

    CString response = CHttpClient::Post(xml, url, false, timeOut);

    //Log.Info("WxPayApi", "Report response : " + response);
    outputObj.FromXml(response);
	return true;
}

/**
* 根据当前系统时间加随机序列来生成订单号
* @return 订单号
*/
CString CWxPayApi::GenerateOutTradeNo(const char* hardId,const char* applyNonceCode)
{
	CString str;
	//str.Format("%d%d%d",CWxPayConfig::MCHID,time(NULL),rand());
	str.Format("%s%s",hardId,applyNonceCode);
	return MD5Str((BYTE*)str.GetBuffer(),str.GetLength());
}

/**
* 生成时间戳，标准北京时间，时区为东八区，自1970年1月1日 0点0分0秒以来的秒数
* @return 时间戳
*/
CString CWxPayApi::GenerateTimeStamp()
{
	DWORD timestamp=(DWORD)time(NULL);
	CString str;
	str.Format("%ld",timestamp);
	return str;
}

/**
* 生成随机串，随机串包含字母或数字
* @return 随机串
*/
CString CWxPayApi::GenerateNonceStr()
{
	GUID guid;
	CoCreateGuid(&guid);
	CString strGuid;
	strGuid.Format("%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",guid.Data1,guid.Data2 ,guid.Data3 ,guid.Data4[0],
		guid.Data4[1],guid.Data4[2],guid.Data4[3],guid.Data4[4],guid.Data4[5],guid.Data4[6],guid.Data4[7]);
	return strGuid;
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
/**
* 生成扫描支付模式一URL
* @param productId 商品ID
* @return 模式一URL
*/
CString CNativePay::GetPrePayUrl(const char* productId)
{
    CWxPayData data;
    data.SetValue("appid", CWxPayConfig::APPID);//公众帐号id
    data.SetValue("mch_id", CWxPayConfig::MCHID);//商户号
    data.SetValue("nonce_str", CWxPayApi::GenerateNonceStr());//随机字符串
    data.SetValue("product_id", productId);//商品ID
	data.SetValue("time_stamp", CWxPayApi::GenerateTimeStamp());//时间戳
    data.SetValue("sign", data.MakeSign());//签名
    CString url = "weixin://wxpay/bizpayurl?" + data.ToUrl(true);
    return url;
}

/**
* 生成直接支付url，支付url有效期为2小时,模式二
* @param productId 商品ID
* @return 模式二URL
*/
CString CNativePay::GetPayUrl(int money,const char* productId,const char* nonce_str,const char* out_trade_no,
							  const char* description/*=NULL*/,const char* attachData/*=NULL*/,const char* tag/*=NULL*/)
{
    CWxPayData data;
    data.SetValue("appid", CWxPayConfig::APPID);//公众账号ID
	data.SetValue("attach", attachData!=NULL?attachData:"");//附加数据CString(127)
	data.SetValue("body", description!=NULL?description:"");//商品描述CString(128)
    data.SetValue("goods_tag", tag!=NULL?tag:"");//商品标记
	data.SetValue("mch_id", CWxPayConfig::MCHID);//商户号
	//异步通知url未设置，则使用配置文件中的url
	if (!data.IsSet("notify_url"))
	{
		data.SetValue("notify_url", CWxPayConfig::NOTIFY_URL);//异步通知url
	}
	data.SetValue("out_trade_no", out_trade_no);//随机字符串	CString(32)
	data.SetValue("product_id", productId);		//商品ID	CString(32)
	data.SetValue("nonce_str", nonce_str);		//随机字符串	CString(32)
    data.SetValue("spbill_create_ip", CWxPayConfig::IP);//终端ip
	CTime startTime=CTime::GetCurrentTime();
	CTime endTim=startTime+CTimeSpan(0,0,10,0);
	data.SetValue("time_expire", endTim.Format("%Y%m%d%H%M%S"));//交易结束时间
	data.SetValue("time_start", startTime.Format("%Y%m%d%H%M%S"));//交易起始时间
	data.SetValue("total_fee", money);//总金额
	data.SetValue("trade_type", "NATIVE");//交易类型
    
	CWxPayData result;
    CWxPayApi::UnifiedOrder(data,result);//调用统一下单接口
    CString url = result.GetValueStr("code_url");//获得统一下单接口返回的二维码链接
    return url;
}