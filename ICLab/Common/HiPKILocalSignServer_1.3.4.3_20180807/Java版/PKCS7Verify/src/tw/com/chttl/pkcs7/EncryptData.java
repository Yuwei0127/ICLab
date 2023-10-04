package tw.com.chttl.pkcs7;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.CertificateEncodingException;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.NoSuchPaddingException;

import org.bouncycastle.cms.CMSAlgorithm;
import org.bouncycastle.cms.CMSEnvelopedData;
import org.bouncycastle.cms.CMSEnvelopedDataGenerator;
import org.bouncycastle.cms.CMSException;
import org.bouncycastle.cms.CMSProcessableByteArray;
import org.bouncycastle.cms.jcajce.JceCMSContentEncryptorBuilder;
import org.bouncycastle.cms.jcajce.JceKeyTransRecipientInfoGenerator;

public class EncryptData {
	String plainData;
	String plainEncoding;
	X509Certificate userCert;
	String cipherType;
	public void setPlainData(String val){
		plainData=val;
	}
	public void setCertb64(String val) throws CertificateException{
		CertificateFactory cf;
		cf = CertificateFactory.getInstance("X509");
		userCert=(X509Certificate)cf.generateCertificate(new ByteArrayInputStream(Base64.decodeLines(val)));
	}
	public void setPlainEncoding(String val){
		plainEncoding=val;
	}
	public String getPlainEncoding(){
		if(plainEncoding!=null && plainEncoding.compareTo("base64")==0) return "base64";
		return "NONE";
	}
	public void setCipherType(String val){
		cipherType=val;
	}
	public String getCipherType(){
		if(cipherType!=null && cipherType.compareTo("PKCS1")==0) return "PKCS1";
		return "PKCS7";
	}
	byte[] encryptP7(byte[] plainBytes) throws CertificateEncodingException, CMSException, IOException{
		CMSProcessableByteArray msg     = new CMSProcessableByteArray(plainBytes);

		CMSEnvelopedDataGenerator edGen = new CMSEnvelopedDataGenerator();
		
		edGen.addRecipientInfoGenerator(new JceKeyTransRecipientInfoGenerator(userCert));
		
		CMSEnvelopedData ed = edGen.generate(
                                   msg,
                                   new JceCMSContentEncryptorBuilder(CMSAlgorithm.DES_EDE3_CBC).build());
		return ed.getEncoded();
	}
	byte[] encryptP1(byte[] plainBytes) throws NoSuchAlgorithmException, NoSuchPaddingException, InvalidKeyException, IllegalBlockSizeException, BadPaddingException
	{
		Cipher c=Cipher.getInstance("RSA/ECB/PKCS1Padding");
		c.init(Cipher.ENCRYPT_MODE, userCert);
		return c.doFinal(plainBytes);
	}
	public String getEncryptedContent(){
		try{
		if(plainData==null || userCert==null) return "No plain data or user certificate found";
			byte[] plainBytes;
			if(this.plainEncoding!=null && plainEncoding.compareTo("base64")==0){
				plainBytes=Base64.decodeLines(plainData);
			}else plainBytes=plainData.getBytes();
			if(this.cipherType!=null && cipherType.compareTo("PKCS1")==0){
				return Base64.encodeLines(encryptP1(plainBytes));
			}
			//default use PKCS#7 to encrypt
			return new String(Base64.encode(encryptP7(plainBytes)));
		}catch(Exception ex){
			System.out.println("Exception in handling plain="+plainData);
			ex.printStackTrace();
			return ex.toString();
		}
	}
}
