<%@ page language="java" contentType="text/html; charset=utf-8"
    pageEncoding="UTF8"%>
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<head>
<meta http-equiv="X-UA-Compatible" content="IE=EDGE" charset="utf-8"/> 
<title>Verify Result</title>
</head>
<body>
<H2>
PKCS7簽章驗證<br/>
<jsp:useBean id="VerifyBean" class="tw.com.chttl.pkcs7.PKCS7Verify" />
<jsp:setProperty name="VerifyBean" property="*"/>
Verify result:<jsp:getProperty property="verifySignature" name="VerifyBean"/><br/>
Issuer:<jsp:getProperty property="issuer" name="VerifyBean"/><br/>
Subject:<jsp:getProperty property="dn" name="VerifyBean"/><br/>
SerialNumber:<jsp:getProperty property="serialNumber" name="VerifyBean"/><br/>
Not After:<jsp:getProperty property="notAfter" name="VerifyBean"/><br/>
Card Number:<jsp:getProperty property="cardSN" name="VerifyBean"/><br/>
Nonce:<jsp:getProperty property="nonce" name="VerifyBean"/><br/>
Sign Time:<jsp:getProperty property="signTime" name="VerifyBean"/><br/>
</H2></body>
</html>