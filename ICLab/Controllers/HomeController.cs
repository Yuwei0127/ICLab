using ICLab.ICSC;
using ICLab.Models;
using System;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;
using System.Web.Mvc;

namespace ICLab.Controllers
{
    public class HomeController : Controller
    {

        public ActionResult Index()
        {
            var iSeed = 123;
            var szClientCerFN = @"cert_SHA1.cer";

            // var lpRawICSData = new ICSDATA
            // {
            //     _ClientID = string.Empty,
            //     _Nonce = string.Empty,
            //     _DateTime = string.Empty,
            //     _Result = 0,
            //     Count = 1
            // };
            //
            // SNPID[] snpidArray = new SNPID[]
            // {
            //     new SNPID
            //     {
            //         SN = "0000000115948071",
            //         PID = "J222563561"
            //         //_Code = 0
            //     }
            // };
            //
            // int structSize = Marshal.SizeOf(typeof(SNPID));
            // lpRawICSData.lpSNPID = Marshal.AllocHGlobal(structSize * lpRawICSData.Count);
            //
            // for (int i = 0; i < lpRawICSData.Count; i++)
            // {
            //     IntPtr ptr = new IntPtr(lpRawICSData.lpSNPID.ToInt64() + i * structSize);
            //     Marshal.StructureToPtr(snpidArray[i], ptr, false);
            // }
            
            
            //----------------------------------------------------------------------------------------------------------
            // // 分配內存並將結構 ICSDATA 複製到內存中
            // IntPtr lpRawICSData = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(ICSDATA)));
            // Marshal.StructureToPtr(new ICSDATA { Count = 1 }, lpRawICSData, false);
            //
            // // 分配內存並將結構 SNPID 複製到內存中
            // IntPtr lpSNPIDArray = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(SNPID)));
            // Marshal.StructureToPtr(new SNPID { SN = "0000000115948071", PID = "J222563561" }, lpSNPIDArray, false);
            //
            // // 將內存中的結構複製到 ICSDATA 結構
            // ICSDATA rawData = new ICSDATA { Count = 1, lpSNPID = lpSNPIDArray };
            // Marshal.StructureToPtr(rawData, lpRawICSData, false);
            //----------------------------------------------------------------------------------------------------------
            
            ICSDATA rawData = new ICSDATA
            {
                _ClientID = "",
                _Nonce = "",
                _DateTime = "",
                _Result = 0,
                Count = 2, // 你的 SNPID 陣列有兩個元素
                lpSNPID = IntPtr.Zero // 先初始化為 IntPtr.Zero
            };

            SNPID[] snpidArray = new SNPID[]
            {
                new SNPID { SN = "0000000115948071", PID = "J222563561" },
                new SNPID { SN = "0000000117251127", PID = "A128346902" }
            };

            // 先分配記憶體並將 rawData 結構轉換為 IntPtr
            IntPtr lpRawICSData = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(ICSDATA)));
            Marshal.StructureToPtr(rawData, lpRawICSData, false);

            // 為 SNPID 陣列分配記憶體
            IntPtr lpSNPIDArray = Marshal.AllocHGlobal(Marshal.SizeOf(typeof(SNPID)) * rawData.Count);

            // 將 SNPID 陣列轉換為 IntPtr
            for (int i = 0; i < rawData.Count; i++)
            {
                IntPtr ptr = new IntPtr(lpSNPIDArray.ToInt64() + i * Marshal.SizeOf(typeof(SNPID)));
                Marshal.StructureToPtr(snpidArray[i], ptr, false);
            }

            // 將 lpSNPID 指向分配的陣列記憶體
            rawData.lpSNPID = lpSNPIDArray;
            //----------------------------------------------------------------------------------------------------------
            
            int iRetPKTLength = 0;
            
            int bufferSize = 2048;
            IntPtr szRetPKT = Marshal.AllocHGlobal(bufferSize);
            // IntPtr szRetPKT = IntPtr.Zero;
            
            try
            {
                // 調用 C 函數，傳遞 ICSDATA 結構
                var result = ICSCService.iMake_ICSReqPKT(iSeed, szClientCerFN, ref lpRawICSData, ref iRetPKTLength, out szRetPKT);

                if (result != 0)
                {
                    int errorCode = Marshal.GetLastWin32Error();
                    Console.WriteLine($"Error code: {errorCode}");
                }
                
                // 釋放szRetPKT
                if (szRetPKT != IntPtr.Zero)
                {
                    Marshal.FreeCoTaskMem(szRetPKT);
                }
                
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
                throw;
            }

            return View();
        }
    }
}