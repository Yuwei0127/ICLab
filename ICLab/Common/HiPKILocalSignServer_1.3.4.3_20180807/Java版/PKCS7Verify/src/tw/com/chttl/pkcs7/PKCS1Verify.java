package tw.com.chttl.pkcs7;

import java.io.ByteArrayInputStream;
import java.security.Signature;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;



public class PKCS1Verify {

	X509Certificate userCert;
	byte[] signature;
	String tbs;
	String tbsEncoding;
	String algorithm;
	
	public void setB64Signature(String sigb64){
		signature=Base64.decode(sigb64);
	}
	public void setB64Cert(String certb64){
		try {
			CertificateFactory cf=CertificateFactory.getInstance("X509");
			userCert=(X509Certificate)cf.generateCertificate(new ByteArrayInputStream(Base64.decode(certb64)));
		} catch (CertificateException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	public void setTbs(String val){
		tbs=val;
		System.out.println("TBS="+tbs);
	}
	public void setTbsEncoding(String val){
		tbsEncoding=val;
	}
	public void setHashAlgorithm(String val){
		algorithm=val;
	}
	public String getDn(){
		if(userCert==null) return "No User Certificate";
		return userCert.getSubjectDN().toString();
	}
	public String getIssuer(){
		if(userCert==null) return "No User Certificate";
		return userCert.getIssuerDN().toString();
	}
	public String getSerialNumber(){
		if(userCert==null) return "No User Certificate";
		return userCert.getSerialNumber().toString(16);
	}
	public String getNotAfter(){
		if(userCert==null) return "No User Certificate";
		return userCert.getNotAfter().toString();
	}
	public boolean getVerifySignature(){
		try {
			Signature s=Signature.getInstance(algorithm+"withRSA");
			s.initVerify(userCert);
			if(tbsEncoding!=null && tbsEncoding.compareTo("base64")==0)
				s.update(Base64.decodeLines(tbs));
			else s.update(tbs.getBytes());
			return s.verify(signature);
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
	}

}
