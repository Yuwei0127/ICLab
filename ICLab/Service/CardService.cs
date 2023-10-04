using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Runtime.InteropServices;
using System.IO;

namespace ICLab.Service
{
    public static class CardService
    {
        // 声明C语言DLL中的函数
        [DllImport(@"D:\Side Project\WebSite\ICLab\ICLab\Common\HISECURE_CryptoModule\ChtHiSECURE5_GPKICardFunction.dll")]
        public static extern int GetCertificateFromGPKICard(int iCertID, byte[] pCertfromICCard,ref int iCertLength,string sReadName);

        public static void Test()//GetCertificateFromGPKICard()
        {
            // 调用C语言DLL中的函数
            // int result = GetCertificateFromGPKICard();
            // Console.WriteLine("Result from C function: " + result);
        }



        public static int GetCardInfo()
        {
            int iRetValue;
            int iCertID = 1; // 取出卡片內部第一張憑證
            byte[] pCertfromICCard = null;
            int iCertLength = 0;
            string sReadName = null;

            try
            {
                // 第一次目的為取得憑證的正確長度
                iRetValue = GetCertificateFromGPKICard(iCertID, pCertfromICCard, ref iCertLength, sReadName);

                // 宣告足夠大的記憶體空間,此函式才能正確動作
                pCertfromICCard = new byte[iCertLength];
                iRetValue = GetCertificateFromGPKICard(iCertID, pCertfromICCard, ref iCertLength, sReadName);

                if (iRetValue != 0)
                {
                    Console.WriteLine("無法取得 IC 卡憑證!");
                    return 1;
                }

                // 写入证书数据到文件
                File.WriteAllBytes("user.cer", pCertfromICCard);

                Console.WriteLine("取得 IC 卡憑證, 內容已存儲到 user.cer 檔案中");

                return 0;
            }
            catch(BadImageFormatException ex) // 64位元的应用程序不能直接引用32位元的DLL 例外
            {
                Console.WriteLine(ex);
                return 1;

            }
            catch (Exception ex)
            {
                Console.WriteLine(ex);
                return 1;
            }           
        }
    }


}