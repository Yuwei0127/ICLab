using ICLab.Models;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Web;
using System.Xml.Linq;

namespace ICLab.Service.ICSC
{
    public class ICSCService
    {
        [DllImport(@"D:\Side Project\WebSite\ICLab\ICLab\Common\DllPackages\ICSCDLL.dll", CallingConvention = CallingConvention.Cdecl)]

        public static extern int iMake_ICSReqPKT(
        int iSeed,
        string szClientCerFN,
        ref ICSDATA lpRawICSData,
        out int iRetPKTLength,
        out IntPtr szRetPKT
    );
    }
}