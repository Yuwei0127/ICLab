<%@ page language="java" contentType="text/html; charset=UTF8"
    pageEncoding="UTF8"%>

<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="X-UA-Compatible" content="IE=EDGE" charset="utf-8"/> 
<title>Decrypt data test</title>
<script type="text/javascript" src="errorcode.js"></script>
<SCRIPT type="text/javascript">
var postTarget;
var timeoutId;
function postData(target,data)
{
	if(!http.sendRequest)
	{
		return null;
	}
	http.url=target;
	http.actionMethod="POST";
	var code=http.sendRequest(data);
	if(code!=0) return null;
	return http.responseText;

}
function checkFinish(){
	if(postTarget){
		postTarget.close();
		alert("尚未安裝元件");
	}
}
function decryptData()
{
    var ua = window.navigator.userAgent;
	if(ua.indexOf("MSIE")!=-1 || ua.indexOf("Trident")!=-1) //is IE, use ActiveX
	{
		postTarget=window.open("waiting.htm?解密中", "Decrypting","height=200, width=200, left=100, top=20");
		var tbsPackage=getEncryptedPackage();
		document.getElementById("httpObject").innerHTML='<OBJECT id="http" width=1 height=1 style="LEFT: 1px; TOP: 1px" type="application/x-httpcomponent" VIEWASTEXT></OBJECT>';
		var data=postData("http://localhost:61161/decrypt","tbsPackage="+tbsPackage);
		postTarget.close();
		postTarget=null;
		if(!data) alert("尚未安裝元件");
		else setDecryptResult(data);
	}
	else{
		postTarget=window.open("http://localhost:61161/popupForm", "解密中","height=200, width=200, left=100, top=20");
		timeoutId=setTimeout(checkFinish,3500);
	}
}

function getEncryptedPackage(){
	var tbsData = {};
	tbsData["cipher"]=encodeURIComponent(document.getElementById("cipher").value.replace(/[\r\n]/g, ''));
	tbsData["cipherType"]=document.getElementById("cipherType").innerHTML;
	tbsData["plainEncoding"]=document.getElementById("plainEncoding").innerHTML;
	tbsData["pin"]=encodeURIComponent(document.getElementById("pin").value);
	tbsData["func"]="DecryptData";
	var json = JSON.stringify(tbsData);
	return json;
}
function setDecryptResult(result)
{
	var ret=JSON.parse(result);
	document.getElementById("plainData").value=ret.plain;
	document.getElementById("returnCode").value=ret.ret_code;
	if(ret.ret_code!=0){
		alert(MajorErrorReason(ret.ret_code));
		if(ret.last_error)
			alert(MinorErrorReason(ret.last_error));
	}
}

function receiveMessage(event)
{
	if(console) console.debug(event);
	
	//安全起見，這邊應填入網站位址檢查
	if(event.origin!="http://localhost:61161")
		return;
	try{
		var ret = JSON.parse(event.data);
		if(ret.func){
			if(ret.func=="getTbs"){
				clearTimeout(timeoutId);
				var json=getEncryptedPackage()
				postTarget.postMessage(json,"*");
			}else if(ret.func=="decrypt"){
				setDecryptResult(event.data);
			}
		}else{
			if(console) console.error("no func");
		}
	}catch(e){
		//errorhandle
		if(console) console.error(e);
	}
}
if (window.addEventListener) {
	window.addEventListener("message", receiveMessage, false);
	}else {
	//for IE8
		window.attachEvent("onmessage", receiveMessage);
	}
	//for IE8
var console=console||{"log":function(){}, "debug":function(){}, "error":function(){}};
</SCRIPT> 
</head>
<body>
<H2>
IC卡解密測試<br/></H2>
<span id="httpObject" ></span>

<jsp:useBean id="EncryptBean" class="tw.com.chttl.pkcs7.EncryptData" />
<jsp:setProperty name="EncryptBean" property="*"/>
已加密資料：<br/>
<TEXTAREA name="cipher" id="cipher" readonly="true" rows="8" cols="65">
<jsp:getProperty property="encryptedContent" name="EncryptBean"/>
</TEXTAREA><br/>
cipher type:<label name="cipherType" id="cipherType"><jsp:getProperty property="cipherType" name="EncryptBean"/></label><br/>
plain encoding:<label name="plainEncoding" id="plainEncoding"><jsp:getProperty property="plainEncoding" name="EncryptBean"/></label><br/>
pin: <INPUT name="pin" id="pin" type="password" value="12345678"><BR>
<INPUT id="DecryptData" type="button" value="解密" onclick="decryptData()"> <br/>
解密結果：<br/><TEXTAREA name="plainData" id="plainData" readonly="true" rows="8" cols="65"></TEXTAREA><br/>
錯誤代碼：<input type="text" name="returnCode" id="returnCode"/>
</body>
</html>