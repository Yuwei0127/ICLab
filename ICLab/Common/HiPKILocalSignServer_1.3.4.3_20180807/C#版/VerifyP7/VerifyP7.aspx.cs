using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Security.Cryptography.Pkcs;
using System.Security.Cryptography;
using System.Text;
namespace VerifyP7
{
    public partial class VerifyP7 : System.Web.UI.Page
    {
        String cardNumber;
        String nonce;
        DateTime signTime;

        private void parseAttribute(SignerInfo si)
        {
            //CryptographicAttributeObject attr;
            foreach(CryptographicAttributeObject attr in si.SignedAttributes)
            {
                AsnEncodedData[] data = new AsnEncodedData[1];
                attr.Values.CopyTo(data, 0);
                if(attr.Oid.Value.CompareTo("1.2.840.113549.1.9.25.3")==0) //Random nonce
                {
                    nonce = Encoding.UTF8.GetString(data[0].RawData, 2, data[0].RawData.Length-2);
                }
                else if (attr.Oid.Value.CompareTo("1.2.840.113549.1.9.5") == 0) //Sign time
                {
                    String signstr = Encoding.UTF8.GetString(data[0].RawData, 2, data[0].RawData.Length - 2);
                    signTime = DateTime.ParseExact(signstr, "yyMMddHHmmssZ", System.Globalization.CultureInfo.InvariantCulture, System.Globalization.DateTimeStyles.AssumeUniversal);
                }
                else if (attr.Oid.Value.CompareTo("2.16.886.1.100.2.204") == 0) //Card number
                {
                    cardNumber = Encoding.UTF8.GetString(data[0].RawData, 2, data[0].RawData.Length - 2);
                }
            }
        }
        protected void Page_Load(object sender, EventArgs e)
        {
            string signedData = Request["b64SignedData"];
            if (signedData==null || signedData.Length==0)
            {
                Response.Write("<HTML><HEAD></HEAD><BODY>No signature found</BODY></HTML>");
                Response.End();
                return;
            }
            byte[] encodedMessage=Convert.FromBase64String(signedData);
            SignedCms signedCms = new SignedCms();
            int certIndex=0;

            // encodedMessage is the encoded message received from 
            // the sender.
            signedCms.Decode(encodedMessage);

            // Verify the signature without validating the 
            // certificate.
            try{
                signedCms.CheckSignature(true);
                Response.Write("<HTML><HEAD></HEAD><BODY>Signature verify successful<br/>");
                Response.Write("TBS:" + Encoding.UTF8.GetString(signedCms.ContentInfo.Content)+"<br/");
                foreach (SignerInfo si in signedCms.SignerInfos)
                {
                    System.Security.Cryptography.X509Certificates.X509Certificate2 x509;
                    x509 = si.Certificate;
                    parseAttribute(si);
                    Response.Write("Cert "+certIndex+":<br/>");
                    Response.Write("Subject:"+x509.Subject+"<br/>");
                    Response.Write("Issuer:"+x509.Issuer+"<br/>");
                    Response.Write("serialNumber:"+x509.SerialNumber+"<br/>");
                    Response.Write("nonce:" + nonce + "<br/>");
                    Response.Write("cardNumber:" + cardNumber + "<br/>");
                    Response.Write("signTime:" + signTime + "<br/><br/>");

                    certIndex++;
                }
                Response.Write("</BODY></HTML>");
            }catch(Exception ex)
            {
                Response.Write("<HTML><HEAD></HEAD><BODY>Signature verify failed, reason="+ex.Message+"</BODY></HTML>");
            }
            Response.End();
        }
    }
}