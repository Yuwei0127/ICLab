using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Web;
using ICLab.Models;

namespace ICLab.Service
{ 
    public class X32Service
    {
        [DllImport("ChtHiSECURE5_GPKICardFunction.dll", EntryPoint = "GetCertificateFromGPKICard", CallingConvention = CallingConvention.Cdecl)]
        public static extern int GetCertificateFromGPKICard(int iCertID, byte[] pCertfromICCard, ref int iCertLength, string sReadName);
        public static void Test()
        {
            int iRetValue;
            int iCertID = 1; // 取出卡片內部第一張憑證
            byte[] pCertfromICCard = new byte[1024];
            int iCertLength = 0;
            string sReadName = "Generic EMV Smartcard Reader 0";

            if (RuntimeInformation.ProcessArchitecture == Architecture.X64)
            {
                // 64-bit application
                Console.WriteLine("Running in 64-bit mode");
                GetCertificateFromGPKICard(iCertID, pCertfromICCard, ref iCertLength, sReadName);
            }
            else if (RuntimeInformation.ProcessArchitecture == Architecture.X86)
            {
                // 32-bit application
                Console.WriteLine("Running in 32-bit mode");
                var resultdata = GetCertificateFromGPKICard(iCertID, pCertfromICCard, ref iCertLength, sReadName);
            }
            else
            {
                // Other architecture
                Console.WriteLine("Unsupported architecture");
            }
        }
        
        // 聲明iMake_ICSReqPKT函式
        [DllImport("D:\\Side Project\\WebSite\\ICLab\\ICLab\\ICSCDLL.dll", CallingConvention = CallingConvention.StdCall)]
        public static extern int iMake_ICSReqPKT(int iSeed, char szClientCerFN, ICSDATA lpRawICSData, ref int iRetPKTLength, out IntPtr szRetPKT);

        public static void GetInfo()
        {
            var dateTimeNow = DateTime.Now.ToString("yyyyMMddhhmmss");
            var seed = 12345;
            var iSeed = BuildSeed(seed);

            var dataModel = new ICSDATA
            {
                _ClientID = "1C6750177E3BB860C1E2B55B635AE48ED00096FE",
                _Nonce = iSeed,
                _DateTime = dateTimeNow,
                _Result = 0,
                Count = 1,
                lpSNPID = new List<SNPID>()
                {
                    new SNPID
                    {
                        SN = "1531302232730052",
                        PID = "F229537342",
                        _Code = 0
                    }
                }
            };

            try
            {

                // // 將 ICSDataModel 轉換為 IntPtr
                // IntPtr dataModelPtr = Marshal.AllocHGlobal(Marshal.SizeOf(dataModel));
                // Marshal.StructureToPtr(dataModel, dataModelPtr, false);

                var 憑證指紋 = "6dab535837b1cf3775f911be99497de42d839952";

                var iRetPKTLength = 0;

                int result = iMake_ICSReqPKT(seed, 'x', dataModel, ref iRetPKTLength, out IntPtr szRetPKT);

                // 處理 result、arg4 和 arg5 的返回值

                // 如果需要，將返回的 IntPtr 轉換回 ICSDataModel
                ICSDATA returnedDataModel = Marshal.PtrToStructure<ICSDATA>(szRetPKT);

                // 釋放內存
                // Marshal.FreeHGlobal(dataModelPtr);
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                throw;
            }
        }

        private static string BuildSeed(int iSeed)
        {
            // 使用 iSeed 作為種子來初始化亂數生成器
            Random random = new Random(iSeed);

            // 生成一個 15 碼的亂數封包識別
            byte[] nonceBytes = new byte[15];
            random.NextBytes(nonceBytes);

            // 將字節數組轉換為十六進制字符串
            var nonce = BitConverter.ToString(nonceBytes).Replace("-", string.Empty);

            return nonce;
        }
    }
}