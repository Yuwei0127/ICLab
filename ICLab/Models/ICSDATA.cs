using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Web;

namespace ICLab.Models
{

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public class ICSDATA
    {
        /// <summary>
        /// 客戶端憑證姆指紋識別碼
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 101)]
        public string _ClientID;

        /// <summary>
        /// 15碼亂數封包識別
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
        public string _Nonce;
        
        /// <summary>
        /// 封包產生時間
        /// </summary>
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
        public string _DateTime;

        /// <summary>
        /// 批次驗證結果
        /// </summary>
        public int _Result;
        
        /// <summary>
        /// 批次待驗之筆數
        /// </summary>
        public int Count;

        /// <summary>
        /// SNPIDData
        /// </summary>
        public SNPID[] lpSNPID;
    }
}