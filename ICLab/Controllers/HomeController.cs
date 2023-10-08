using ICLab.ICSC;
using ICLab.Models;
using ICLab.Service;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Web;
using System.Web.Http.Results;
using System.Web.Mvc;

namespace ICLab.Controllers
{
    public class HomeController : Controller
    {

        public ActionResult Index()
        {
            string szClientCerFN = "1C6750177E3BB860C1E2B55B635AE48ED00096FE"; // 客戶端證書文件名稱
            var dateTimeNow = DateTime.Now.ToString("yyyyMMddhhmmss");
            var iSeed = 12345;
            var _Nonce = BuildSeed(iSeed);

            var lpRawICSData = new ICSDATA
            {
                _ClientID = "1C6750177E3BB860C1E2B55B635AE48ED00096FE",
                _Nonce = _Nonce,
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


            int iRetPKTLength = 0;
            // IntPtr szRetPKT = IntPtr.Zero; 

            int bufferSize = 1024; // 假設資料大小為 100 個字節
            IntPtr szRetPKT = Marshal.AllocHGlobal(bufferSize);

            // 調用C語言函數，傳遞ICSDATA結構的實例
            int result = ICSCService.iMake_ICSReqPKT(iSeed, szClientCerFN, ref lpRawICSData, ref iRetPKTLength, ref szRetPKT);

            // 處理結果和szRetPKT（根據需要）

            // 釋放szRetPKT（如果需要的話）
            if (szRetPKT != IntPtr.Zero)
            {
                Marshal.FreeCoTaskMem(szRetPKT);
            }



            return View();
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