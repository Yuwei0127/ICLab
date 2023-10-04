package tw.com.chttl.pkcs7;

import java.security.cert.X509Certificate;
import java.text.SimpleDateFormat;
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;

import org.bouncycastle.asn1.ASN1ObjectIdentifier;
import org.bouncycastle.asn1.cms.AttributeTable;
import org.bouncycastle.asn1.cms.Attribute;
import org.bouncycastle.cert.X509CertificateHolder;
import org.bouncycastle.cert.jcajce.JcaX509CertificateConverter;
import org.bouncycastle.cms.CMSException;
import org.bouncycastle.cms.CMSSignedData;
import org.bouncycastle.cms.SignerInformation;
import org.bouncycastle.cms.SignerInformationStore;
import org.bouncycastle.cms.jcajce.JcaSimpleSignerInfoVerifierBuilder;
import org.bouncycastle.util.Store;

public class PKCS7Verify {

	CMSSignedData cms;
	X509Certificate userCert;
	Date signTime;
	String cardNumber;
	String nonce;
	public final ASN1ObjectIdentifier NONCE_OID=new ASN1ObjectIdentifier("1.2.840.113549.1.9.25.3");
	public final ASN1ObjectIdentifier SIGNTIME_OID=new ASN1ObjectIdentifier("1.2.840.113549.1.9.5");
	public final ASN1ObjectIdentifier CARDSN_OID=new ASN1ObjectIdentifier("2.16.886.1.100.2.204");
	
	public void setB64SignedData(String sigb64){
		byte[] sig=Base64.decode(sigb64);
		try {
			cms = new CMSSignedData(sig);
			System.out.println("Got signature");
		} catch (CMSException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	public String getCardSN(){return cardNumber;}
	public String getNonce(){return nonce;}
	public Date getSignTime(){return signTime;}
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
	        Store store = cms.getCertificates(); 
	        SignerInformationStore signers = cms.getSignerInfos(); 
	        Collection c = signers.getSigners(); 
	        Iterator it = c.iterator();
	        while (it.hasNext()) { 
	            SignerInformation signer = (SignerInformation) it.next(); 
	            Collection certCollection = store.getMatches(signer.getSID()); 
	            Iterator certIt = certCollection.iterator();
	            X509CertificateHolder certHolder = (X509CertificateHolder) certIt.next();
	            X509Certificate cert = new JcaX509CertificateConverter().getCertificate(certHolder);
	            if (signer.verify(new JcaSimpleSignerInfoVerifierBuilder().build(cert))==false) {
	                return false;
	            }
	            userCert=cert;
	            AttributeTable table=signer.getSignedAttributes();
	            if(table!=null){ 
		            Attribute at=table.get(NONCE_OID);
		            if(at!=null) nonce=at.getAttributeValues()[0].toString();
		            at=table.get(CARDSN_OID);
		            if(at!=null) cardNumber=at.getAttributeValues()[0].toString();
		            at=table.get(SIGNTIME_OID);
		            if(at!=null)
		            {
		            	String dateStr=at.getAttributeValues()[0].toString();
		            	SimpleDateFormat iso8601 = new SimpleDateFormat("yyMMddHHmmssX");
		            	this.signTime=iso8601.parse(dateStr);
		            }
	            }
	        }
	        byte[] data=(byte[])cms.getSignedContent().getContent();
	        System.out.println("TBS="+new String(data,"UTF8"));
	        
	        return true;
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
	}
}
