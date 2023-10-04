using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Security.Cryptography.Pkcs;

namespace VerifyP7
{
    public partial class decryptData : System.Web.UI.Page
    {
        String encryptedData;
        String plainEncoding;
        String cipherType;

        public String getEncryptedData()
        {
            return encryptedData;
        }
        public String getCipherType()
        {
            return cipherType;
        }
        public String getPlainEncoding()
        {
            return plainEncoding;
        }
        protected void Page_Load(object sender, EventArgs e)
        {
            this.encryptedData = "1234";
            String plainData = Request["plainData"];
            String certB64 = Request["certB64"];
            plainEncoding = Request["plainEncoding"];
            cipherType = Request["cipherType"];

            if (plainData == null || plainData.Length == 0 || certB64==null || certB64.Length==0)
            {
                Response.Write("<HTML><HEAD></HEAD><BODY>No plain data and certificate found</BODY></HTML>");
                Response.End();
                return;
            }
            if (plainEncoding == null || plainEncoding.CompareTo("base64")!=0) plainEncoding = "NONE";
            if (cipherType == null || cipherType.CompareTo("PKCS1")!=0) cipherType = "PKCS7";
            byte[] bplain;
            if (plainEncoding != null && plainEncoding.CompareTo("base64") == 0)
            {
                bplain = Convert.FromBase64String(plainData);
            }else bplain=System.Text.Encoding.UTF8.GetBytes(plainData);
            X509Certificate2 x509 = new System.Security.Cryptography.X509Certificates.X509Certificate2(Convert.FromBase64String(certB64));
            if (cipherType != null && cipherType.CompareTo("PKCS1") == 0)
            {
                RSACryptoServiceProvider RSA = (RSACryptoServiceProvider)x509.PublicKey.Key;
                byte[] encrypted = RSA.Encrypt(bplain, false);
                if (encrypted == null)
                {
                    Response.Write("<HTML><HEAD></HEAD><BODY>Encryption failed</BODY></HTML>");
                    Response.End();
                    return;
                }
                this.encryptedData = Convert.ToBase64String(encrypted);
            }
            else //PKCS7
            {
                ContentInfo contentInfo = new ContentInfo(bplain);
                EnvelopedCms enveloped = new EnvelopedCms(contentInfo);
                CmsRecipient recipient=new CmsRecipient(x509);
                enveloped.Encrypt(recipient);
                byte[] encrypted=enveloped.Encode();
                this.encryptedData = Convert.ToBase64String(encrypted);
            }
        }
    }
}