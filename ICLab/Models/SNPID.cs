using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Web;

namespace ICLab.Models
{

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public class SNPID
    {
        /// <summary>
        /// 16碼DN序號
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 17)]
        public string SN;

        /// <summary>
        /// 10碼身分證字號
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 11)]
        public string PID;

        /// <summary>
        /// 本筆資料對查詢結果代碼
        /// </summary>
        public int _Code;
    }
}