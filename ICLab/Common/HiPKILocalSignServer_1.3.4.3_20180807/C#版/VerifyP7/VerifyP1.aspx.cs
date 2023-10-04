using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
namespace VerifyP7
{
    public partial class VerifyP1 : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {

            String signB64 = Request["b64Signature"];
            String certB64 = Request["b64Cert"];
            String tbs = Request["tbs"];
            String hashAlgorithm = Request["hashAlgorithm"];


            if (signB64 == null || signB64.Length == 0)
            {
                Response.Write("<HTML><HEAD></HEAD><BODY>No signature found</BODY></HTML>");
                Response.End();
                return;
            }
            byte[] signature = Convert.FromBase64String(signB64);
            byte[] btbs=System.Text.Encoding.UTF8.GetBytes(tbs);
            X509Certificate2 x509 = new X509Certificate2(Convert.FromBase64String(certB64));
            RSACryptoServiceProvider RSA = (RSACryptoServiceProvider)x509.PublicKey.Key;
            bool result = RSA.VerifyData(btbs, hashAlgorithm, signature);
            if (result == false)
            {
                Response.Write("<HTML><HEAD></HEAD><BODY>Signature verify failed</BODY></HTML>");
                Response.End();
                return;
            }
            Response.Write("<HTML><HEAD></HEAD><BODY>Signature verify successful<br/>");
            Response.Write("Subject:"+x509.Subject+"<br/>");
            Response.Write("Issuer:"+x509.Issuer+"<br/>");
            Response.Write("serialNumber:"+x509.SerialNumber+"<br/><br/>");
            Response.Write("</BODY></HTML>");
            Response.End();
        }
    }
}